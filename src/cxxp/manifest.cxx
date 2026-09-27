#include <cxxp/manifest.hxx>
#include <cxxp/schema.hxx>

#include <memory>

#include <valijson/adapters/nlohmann_json_adapter.hpp>
#include <valijson/schema.hpp>
#include <valijson/schema_parser.hpp>
#include <valijson/validation_results.hpp>
#include <valijson/validator.hpp>

namespace cxxp {
std::vector<ManifestError> validate_manifest(nlohmann::json const &manifest) {
  if (manifest.is_object()) {
    auto const identifier = manifest.find("schemaVersion");
    if (identifier == manifest.end()) {
      return {{ManifestErrorKind::schema_identifier, "schemaVersion",
               "Required integer schemaVersion is missing"}};
    }
    if (!identifier->is_number_integer()) {
      return {{ManifestErrorKind::schema_identifier, "schemaVersion",
               "schemaVersion must be an integer"}};
    }
    if (*identifier != 1) {
      return {{ManifestErrorKind::unsupported_revision, "schemaVersion",
               "Unsupported schemaVersion; expected 1"}};
    }
  }

  static auto const schema_document = nlohmann::json::parse(manifest_schema);
  static auto const schema = [] {
    auto result = std::make_unique<valijson::Schema>();
    valijson::SchemaParser parser(valijson::SchemaParser::kDraft7);
    parser.populateSchema(
        valijson::adapters::NlohmannJsonAdapter(schema_document), *result);
    return result;
  }();
  valijson::Validator validator(valijson::Validator::kStrongTypes);
  valijson::ValidationResults results;
  validator.validate(*schema, valijson::adapters::NlohmannJsonAdapter(manifest),
                     &results);
  std::vector<ManifestError> errors;
  valijson::ValidationResults::Error error;
  while (results.popError(error)) {
    std::string field;
    auto const *constraint = &schema_document;
    for (auto const &part : error.context) {
      if (part == "<root>") {
        continue;
      }
      auto name = part.substr(1, part.size() - 2);
      // Valijson 1.1 uses JSON-quoted property names inside context brackets.
      if (name.size() >= 2 && name.front() == '"' && name.back() == '"') {
        name = nlohmann::json::parse(name).get<std::string>();
      }
      if (!field.empty()) {
        field += '.';
      }
      field += name;
      if (constraint->contains("properties") &&
          (*constraint)["properties"].contains(name)) {
        constraint = &(*constraint)["properties"][name];
      }
    }
    if (field.empty()) {
      field = "$";
    }
    if (error.description.find("'type' constraint") != std::string::npos) {
      error.description =
          "Expected " + (*constraint)["type"].get<std::string>();
    }
    auto const missing = error.description.find("Missing required property '");
    auto const unknown = error.description.find("constraints: '");
    if (missing != std::string::npos || unknown != std::string::npos) {
      auto const start =
          error.description.find('\'', missing != std::string::npos ? missing
                                                                    : unknown) +
          1;
      auto const end = error.description.find('\'', start);
      auto const name = error.description.substr(start, end - start);
      field = field == "$" ? name : field + '.' + name;
      if (unknown != std::string::npos) {
        error.description = "Unknown field is not allowed: " + name;
      }
    }
    errors.push_back({ManifestErrorKind::field, field, error.description});
  }
  return errors;
}
} // namespace cxxp
