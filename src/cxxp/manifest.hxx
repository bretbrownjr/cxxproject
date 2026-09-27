#ifndef CXXP_MANIFEST_HXX
#define CXXP_MANIFEST_HXX

#include <nlohmann/json.hpp>
#include <string>
#include <vector>

namespace cxxp {

enum class ManifestErrorKind { field, schema_identifier, unsupported_revision };

struct ManifestError {
  ManifestErrorKind kind;
  std::string field;
  std::string message;
};

// Returns zero or more errors from validating a manifest
std::vector<ManifestError> validate_manifest(nlohmann::json const &manifest);

} // namespace cxxp

#endif // CXXP_MANIFEST_HXX
