#ifndef CXXP_PROCESS_HXX
#define CXXP_PROCESS_HXX

#include <filesystem>
#include <string>
#include <vector>

namespace cxxp {

struct ProcessResult {
  int exit_code = 0;
  int signal = 0;
  std::string output;
  bool succeeded() const { return exit_code == 0 && signal == 0; }
};

// Resolve against the caller's cwd and PATH, before entering a project.
std::filesystem::path resolve_executable(std::filesystem::path const &name);

// Requires an absolute executable and working directory. Launch/OS failures
// throw; child exits and signals are returned. Stderr is inherited, stdin is
// /dev/null, and stdout is discarded unless capture_stdout is true.
ProcessResult run_process(std::filesystem::path const &executable,
                          std::vector<std::string> const &arguments,
                          std::filesystem::path const &working_directory,
                          bool capture_stdout = false);

} // namespace cxxp

#endif
