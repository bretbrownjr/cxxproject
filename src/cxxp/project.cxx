#include <cxxp/project.hxx>

#include <cxxp/project_file.hxx>

#include <algorithm>
#include <array>
#include <fstream>
#include <map>
#include <set>
#include <stdexcept>
#include <string_view>
#include <system_error>

namespace cxxp {
namespace {
using Json = nlohmann::json;

Json read_json(std::filesystem::path const &path) {
  std::ifstream input(path);
  if (!input) {
    throw std::runtime_error("cannot read '" + path.string() + "'");
  }
  std::map<int, std::set<std::string>> keys;
  auto const callback = [&keys, &path](int depth, Json::parse_event_t event,
                                       Json &parsed) {
    if (event == Json::parse_event_t::object_start) {
      keys[depth + 1].clear();
    } else if (event == Json::parse_event_t::key &&
               !keys[depth].insert(parsed.get<std::string>()).second) {
      throw std::runtime_error("'" + path.string() +
                               "': duplicate JSON object key '" +
                               parsed.get<std::string>() + "'");
    }
    return true;
  };
  try {
    return Json::parse(input, callback);
  } catch (Json::parse_error const &error) {
    throw std::runtime_error("'" + path.string() +
                             "': malformed JSON: " + error.what());
  }
}

bool has_extension(std::filesystem::path const &path,
                   std::array<std::string_view, 4> const &extensions) {
  auto const extension = path.extension().string();
  return std::ranges::find(extensions, extension) != extensions.end();
}

std::string relative_path(std::filesystem::path const &root,
                          std::filesystem::path const &path) {
  return path.lexically_relative(root).generic_string();
}

void sort(std::vector<std::string> &paths) { std::ranges::sort(paths); }

void discover(Project &project) {
  namespace fs = std::filesystem;
  static constexpr std::array<std::string_view, 4> source_extensions{
      ".c", ".cc", ".cpp", ".cxx"};
  static constexpr std::array<std::string_view, 4> header_extensions{
      ".h", ".hh", ".hpp", ".hxx"};

  std::error_code error;
  fs::recursive_directory_iterator iterator(project.root, error), end;
  if (error) {
    throw std::runtime_error("cannot scan '" + project.root.string() +
                             "': " + error.message());
  }
  while (iterator != end) {
    auto const path = iterator->path();
    auto const relative = relative_path(project.root, path);
    auto const status = iterator->symlink_status(error);
    if (error) {
      throw std::runtime_error("cannot inspect '" + path.string() +
                               "': " + error.message());
    }
    if (fs::is_symlink(status)) {
      iterator.disable_recursion_pending();
    } else if (fs::is_directory(status)) {
      auto const name = path.filename().string();
      auto excluded = name == ".git" || name == ".hg" || name == ".svn" ||
                      name == "build" || name.starts_with("cmake-build-");
      if (!excluded) {
        auto const project_file = path / "cxx.json";
        auto const cache = path / "CMakeCache.txt";
        auto const nested_project = fs::exists(project_file, error);
        if (error) {
          throw std::runtime_error("cannot inspect '" + project_file.string() +
                                   "': " + error.message());
        }
        auto const has_cache = fs::exists(cache, error);
        if (error) {
          throw std::runtime_error("cannot inspect '" + cache.string() +
                                   "': " + error.message());
        }
        excluded = nested_project || has_cache;
      }
      if (excluded) {
        iterator.disable_recursion_pending();
      }
    } else if (fs::is_regular_file(status)) {
      if (has_extension(path, source_extensions)) {
        project.sources.push_back(relative);
      } else if (has_extension(path, header_extensions)) {
        project.headers.push_back(relative);
      }
      auto const name = path.filename().string();
      if (name == ".clang-format" || name == "_clang-format") {
        project.clang_format_configurations.push_back(relative);
      } else if (name == ".clang-tidy") {
        project.clang_tidy_configurations.push_back(relative);
      }
      if (relative == "CMakeLists.txt") {
        project.backend = BuildBackend::cmake;
      }
    }
    iterator.increment(error);
    if (error) {
      throw std::runtime_error("cannot scan '" + path.string() +
                               "': " + error.message());
    }
  }
  sort(project.sources);
  sort(project.headers);
  sort(project.clang_format_configurations);
  sort(project.clang_tidy_configurations);
}
} // unnamed namespace

Project load_project(std::filesystem::path const &start) {
  namespace fs = std::filesystem;
  std::error_code error;
  auto current = fs::canonical(start, error);
  if (error) {
    throw std::runtime_error("cannot resolve starting directory '" +
                             start.string() + "': " + error.message());
  }
  if (!fs::is_directory(current, error) || error) {
    throw std::runtime_error("starting path is not a directory: '" +
                             current.string() + "'");
  }

  auto root = current;
  auto project_file_path = fs::path{};
  for (;;) {
    auto const candidate = root / "cxx.json";
    auto const status = fs::symlink_status(candidate, error);
    if (error && error != std::errc::no_such_file_or_directory) {
      throw std::runtime_error("cannot inspect '" + candidate.string() +
                               "': " + error.message());
    }
    error.clear();
    if (status.type() != fs::file_type::not_found) {
      project_file_path = candidate;
      break;
    }
    auto const parent = root.parent_path();
    if (parent == root) {
      throw std::runtime_error("no cxx.json found from '" + current.string() +
                               "' or its parent directories");
    }
    root = parent;
  }

  auto const document = read_json(project_file_path);
  auto const errors = validate_project_file(document);
  if (!errors.empty()) {
    auto const &issue = errors.front();
    throw std::runtime_error("'" + project_file_path.string() +
                             "': " + issue.field + ": " + issue.message);
  }

  Project project{root, document.at("project").at("name").get<std::string>(),
                  document.at("project").at("version").get<std::string>()};
  discover(project);
  return project;
}

} // namespace cxxp
