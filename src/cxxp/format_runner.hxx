#ifndef CXXP_FORMAT_RUNNER_HXX
#define CXXP_FORMAT_RUNNER_HXX

#include <cxxp/format_backend.hxx>

#include <filesystem>
#include <vector>

namespace cxxp {

// Call only after frontend policy checks. Files must be absolute. Stops at the
// first failure, with the tool and file in the exception diagnostic.
void run_formatter(std::filesystem::path const &executable,
                   std::filesystem::path const &project_root,
                   std::vector<std::filesystem::path> files,
                   FormatOperation operation);

} // namespace cxxp
#endif
