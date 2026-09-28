#include <cerrno>
#include <cstdlib>
#include <cxxp/format_backend.hxx>
#include <cxxp/json_file.hxx>
#include <cxxp/lock_file.hxx>
#include <cxxp/project_file.hxx>
#include <cxxp/schema.hxx>
#include <stdexcept>
#include <system_error>
#include <unistd.h>
#include <vector>

namespace cxxp {

namespace {

void validate(nlohmann::json const &document) {
  auto const errors = validate_document(document, lock_file_schema);
  if (!errors.empty()) {
    throw std::runtime_error("invalid cxx.lock.json: " + errors.front().field +
                             ": " + errors.front().message);
  }
  for (auto const &[tool, record] : document.at("developmentTools").items()) {
    if (tool.empty()) {
      throw std::runtime_error("invalid cxx.lock.json: empty tool identifier");
    }
    if (tool == ClangFormatBackend::tool_id &&
        !ClangFormatBackend::valid_release(
            record.at("version").get<std::string>())) {
      throw std::runtime_error("invalid clang-format release in cxx.lock.json");
    }
  }
}

} // unnamed namespace

LockFile::LockFile()
    : document_{{"schemaVersion", 1},
                {"projectDependencies", nlohmann::json::object()},
                {"developmentTools", nlohmann::json::object()}} {}

LockFile::LockFile(nlohmann::json document) : document_(std::move(document)) {
  validate(document_);
}

std::optional<LockFile> LockFile::load(std::filesystem::path const &path) {
  // A dangling symlink is an invalid existing lock, not a missing lock.
  if (std::filesystem::symlink_status(path).type() ==
      std::filesystem::file_type::not_found) {
    return std::nullopt;
  }
  return LockFile(read_json(path));
}

nlohmann::json const &LockFile::document() const { return document_; }

std::optional<std::string> LockFile::version(std::string_view tool) const {
  auto const &tools = document_.at("developmentTools");
  auto const found = tools.find(std::string(tool));
  if (found == tools.end())
    return std::nullopt;
  return found->at("version").get<std::string>();
}

void LockFile::require_tools(std::span<std::string_view const> tools) const {
  for (auto tool : tools) {
    if (!version(tool))
      throw std::runtime_error("cxx.lock.json is missing required tool '" +
                               std::string(tool) + "'");
  }
}

void LockFile::set_tool(std::string_view tool, std::string_view version) {
  auto updated = document_;
  updated["developmentTools"][std::string(tool)] = {{"version", version}};
  validate(updated);
  document_ = std::move(updated);
}

void LockFile::write_atomic(std::filesystem::path const &path) const {
  auto const bytes = document_.dump(2) + '\n';
  auto pattern = path.string() + ".tmp.XXXXXX";
  std::vector<char> name(pattern.begin(), pattern.end());
  name.push_back('\0');
  int fd = ::mkstemp(name.data());
  if (fd < 0)
    throw std::system_error(errno, std::generic_category(),
                            "cannot create temporary lock for " +
                                path.string());
  try {
    std::size_t offset = 0;
    while (offset < bytes.size()) {
      auto const count =
          ::write(fd, bytes.data() + offset, bytes.size() - offset);
      if (count < 0 && errno == EINTR)
        continue;
      if (count <= 0)
        throw std::system_error(count == 0 ? EIO : errno,
                                std::generic_category(), "cannot write lock");
      offset += static_cast<std::size_t>(count);
    }
    if (::fsync(fd) != 0)
      throw std::system_error(errno, std::generic_category(),
                              "cannot flush lock");
    auto const closed = ::close(fd);
    fd = -1;
    if (closed != 0)
      throw std::system_error(errno, std::generic_category(),
                              "cannot close lock");
    std::filesystem::rename(name.data(), path);
  } catch (...) {
    if (fd >= 0)
      ::close(fd);
    ::unlink(name.data());
    throw;
  }
}

} // namespace cxxp
