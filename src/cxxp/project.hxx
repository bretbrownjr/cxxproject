#ifndef CXXP_PROJECT_HXX
#define CXXP_PROJECT_HXX

#include <filesystem>
#include <string>
#include <vector>

namespace cxxp {

// Identifies the build backend detected for a project.
enum class BuildBackend { none, cmake };

// Holds project metadata and the files and tools discovered under its root.
struct Project {
  std::filesystem::path root;
  std::string name;
  std::string version;
  std::vector<std::string> sources;
  std::vector<std::string> headers;
  std::vector<std::string> clang_format_configurations;
  std::vector<std::string> clang_tidy_configurations;
  BuildBackend backend = BuildBackend::none;
};

// Finds and loads the nearest project starting at the given directory.
Project load_project(
    std::filesystem::path const &start = std::filesystem::current_path());

} // namespace cxxp

#endif // CXXP_PROJECT_HXX
