#include <cxxp/format_runner.hxx>
#include <cxxp/process.hxx>

#include <algorithm>
#include <stdexcept>

namespace cxxp {

void run_formatter(std::filesystem::path const &executable,
                   std::filesystem::path const &project_root,
                   std::vector<std::filesystem::path> files,
                   FormatOperation operation) {
  // Frontend policy checks must finish before invoking the runner.
  for (auto const &file : files) {
    if (!file.is_absolute())
      throw std::invalid_argument("formatter requires absolute file paths");
  }
  std::ranges::sort(files);
  std::string failures;
  for (auto const &file : files) {
    ProcessResult result;
    try {
      result = run_process(executable,
                           ClangFormatBackend::arguments(operation, file),
                           project_root);
    } catch (std::exception const &error) {
      // Launch errors always stop processing.
      throw std::runtime_error(failures + executable.string() + ": " +
                               file.string() + ": " + error.what());
    }
    if (!result.succeeded()) {
      // Check aggregates child failures.
      failures +=
          executable.string() + ": " + file.string() +
          (result.signal
               ? ": terminated by signal " + std::to_string(result.signal)
               : ": exited with status " + std::to_string(result.exit_code)) +
          ". Fix the reported configuration or source error and rerun "
          "formatting; "
          "for formatting differences, run cxxp format.\n";
      if (operation == FormatOperation::rewrite) {
        // Rewrite stops at the first failure.
        throw std::runtime_error(failures);
      }
    }
  }
  if (!failures.empty())
    throw std::runtime_error(failures);
}

} // namespace cxxp
