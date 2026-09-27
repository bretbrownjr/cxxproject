#ifndef CXXP_PROJECT_INFO_HXX
#define CXXP_PROJECT_INFO_HXX

#include <cxxp/project.hxx>

#include <nlohmann/json.hpp>

namespace cxxp {

// Converts a project into the versioned info command output.
void to_json(nlohmann::ordered_json &output, Project const &project);

} // namespace cxxp

#endif // CXXP_PROJECT_INFO_HXX
