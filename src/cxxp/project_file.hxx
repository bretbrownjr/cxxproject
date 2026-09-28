#ifndef CXXP_PROJECT_FILE_HXX
#define CXXP_PROJECT_FILE_HXX

#include <nlohmann/json.hpp>
#include <string>
#include <string_view>
#include <vector>

namespace cxxp {

enum class ProjectFileErrorKind {
  field,
  schema_identifier,
  unsupported_revision
};

struct ProjectFileError {
  ProjectFileErrorKind kind;
  std::string field;
  std::string message;
};

std::vector<ProjectFileError> validate_document(nlohmann::json const &document,
                                                std::string_view schema_text);

// Returns zero or more errors from validating a cxx.json document.
std::vector<ProjectFileError>
validate_project_file(nlohmann::json const &document);

} // namespace cxxp

#endif // CXXP_PROJECT_FILE_HXX
