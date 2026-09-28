#include <cxxp/format_runner.hxx>
#include <cxxp/process.hxx>

#include <algorithm>
#include <stdexcept>

namespace cxxp {

void run_formatter(std::filesystem::path const &executable,
                   std::filesystem::path const &project_root,
                   std::vector<std::filesystem::path> files,
                   FormatOperation operation) {
  for (auto const &file : files) {
    if (!file.is_absolute())
      throw std::invalid_argument("formatter requires absolute file paths");
  }
  std::ranges::sort(files);
  for (auto const &file : files) {
    try {
      auto result = run_process(executable,
                                ClangFormatBackend::arguments(operation, file),
                                project_root);
      if (!result.succeeded()) {
        throw std::runtime_error(
            result.signal
                ? "terminated by signal " + std::to_string(result.signal)
                : "exited with status " + std::to_string(result.exit_code));
      }
    } catch (std::exception const &error) {
      throw std::runtime_error(executable.string() + ": " + file.string() +
                               ": " + error.what());
    }
  }
}

} // namespace cxxp
