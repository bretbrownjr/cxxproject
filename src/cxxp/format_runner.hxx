#ifndef CXXP_FORMAT_RUNNER_HXX
#define CXXP_FORMAT_RUNNER_HXX

#include <cxxp/format_backend.hxx>

#include <filesystem>
#include <vector>

namespace cxxp {

// Rewrite or check the given absolute file paths with the formatter, using
// project_root as its working directory and throwing on failure.
void run_formatter(std::filesystem::path const &executable,
                   std::filesystem::path const &project_root,
                   std::vector<std::filesystem::path> files,
                   FormatOperation operation);

} // namespace cxxp
#endif
