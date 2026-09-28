#include <cxxp/format_backend.hxx>

#include <regex>
#include <stdexcept>

namespace cxxp {

bool ClangFormatBackend::valid_release(std::string_view version) {
  static std::regex const release(
      R"((0|[1-9][0-9]*)\.(0|[1-9][0-9]*)\.(0|[1-9][0-9]*))");

  return std::regex_match(version.begin(), version.end(), release);
}

bool ClangFormatBackend::supports(std::string_view version) {
  return version == "22.1.8";
}

ToolIdentity ClangFormatBackend::identify(std::string const &output) {
  static std::regex const identity(
      R"(clang-format version ([0-9]+\.[0-9]+\.[0-9]+)(?=\s|$))");

  auto begin = std::sregex_iterator(output.begin(), output.end(), identity);
  auto end = std::sregex_iterator();

  if (begin == end) {
    throw std::runtime_error("unrecognized clang-format version: " + output);
  }

  auto version = (*begin)[1].str();
  if (++begin != end || !valid_release(version)) {
    throw std::runtime_error("ambiguous or invalid clang-format version: " +
                             output);
  }

  return {version, output};
}

std::vector<std::string> ClangFormatBackend::identification_arguments() {
  return {"--version"};
}

std::vector<std::string>
ClangFormatBackend::arguments(FormatOperation operation,
                              std::filesystem::path const &file) {
  if (!file.is_absolute()) {
    throw std::invalid_argument("formatter requires an absolute file path");
  }

  std::vector<std::string> result{"--style=file",
                                  "--fail-on-incomplete-format"};
  if (operation == FormatOperation::check) {
    result.insert(result.end(), {"--dry-run", "--Werror"});
  } else {
    result.push_back("-i");
  }

  result.push_back(file.string());

  return result;
}

} // namespace cxxp
