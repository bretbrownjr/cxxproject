#ifndef CXXP_FORMAT_PROJECT_HXX
#define CXXP_FORMAT_PROJECT_HXX

#include <cxxp/format_backend.hxx>

#include <filesystem>

namespace cxxp {

// Discover the project from start and rewrite or check its sources and headers,
// requiring an existing formatter lock when locked is true.
void format_project(std::filesystem::path const &start,
                    FormatOperation operation, bool locked = false);

// Discover the project from start and ensure its formatter is locked, replacing
// an existing tool version only when update_tool is true.
void lock_project(std::filesystem::path const &start, bool update_tool = false);

} // namespace cxxp

#endif
