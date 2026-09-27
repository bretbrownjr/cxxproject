#include <cxxp/manifest.hxx>

#if __has_include(<catch2/catch_test_macros.hpp>)
#include <catch2/catch_test_macros.hpp>
#else
#include <catch2/catch.hpp>
#endif

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <string>
#include <string_view>
#include <vector>

namespace cxxp {
namespace {
nlohmann::json read_manifest(std::filesystem::path const &path) {
  INFO("Manifest path: " << path);
  std::ifstream input(path);
  REQUIRE(input.is_open());
  return nlohmann::json::parse(input);
}

void require_diagnostic(std::vector<ManifestError> const &errors,
                        ManifestErrorKind kind, std::string_view field,
                        std::string_view message) {
  REQUIRE_FALSE(errors.empty());
  std::string diagnostics;
  for (auto const &error : errors) {
    diagnostics += error.field + ": " + error.message + '\n';
  }
  INFO(diagnostics);
  CAPTURE(kind, field, message);
  REQUIRE(std::ranges::any_of(errors, [=](auto const &error) {
    return error.kind == kind && error.field.find(field) != std::string::npos &&
           error.message.find(message) != std::string::npos;
  }));
}
} // unnamed namespace

SCENARIO("A complete manifest is accepted", "[manifest]") {
  GIVEN("a manifest with schema revision 1 and a project name and version") {
    auto const manifest =
        read_manifest(std::filesystem::path(CXXP_TEST_FIXTURES) / "valid.json");
    WHEN("the manifest is validated") {
      auto const errors = validate_manifest(manifest);
      THEN("no validation errors are reported") { REQUIRE(errors.empty()); }
    }
  }
}

SCENARIO("Project versions may be arbitrary nonempty strings", "[manifest]") {
  GIVEN("a project version containing words, spaces, and a slash") {
    auto const manifest = read_manifest(
        std::filesystem::path(CXXP_TEST_FIXTURES) / "arbitrary-version.json");
    WHEN("the manifest is validated") {
      auto const errors = validate_manifest(manifest);
      THEN("the version is accepted without imposing a versioning scheme") {
        REQUIRE(errors.empty());
      }
    }
  }
}

SCENARIO("A missing schema identifier is rejected", "[manifest]") {
  GIVEN("a manifest without a schema identifier") {
    auto const manifest =
        read_manifest(std::filesystem::path(CXXP_TEST_FIXTURES) /
                      "missing-schemaVersion.json");
    WHEN("the manifest is validated") {
      auto const errors = validate_manifest(manifest);
      THEN("the diagnostic identifies the missing schema identifier") {
        require_diagnostic(errors, ManifestErrorKind::schema_identifier,
                           "schemaVersion", "schemaVersion");
      }
    }
  }
}

SCENARIO("A missing project object is rejected", "[manifest]") {
  GIVEN("a manifest without a project object") {
    auto const manifest = read_manifest(
        std::filesystem::path(CXXP_TEST_FIXTURES) / "missing-project.json");
    WHEN("the manifest is validated") {
      auto const errors = validate_manifest(manifest);
      THEN("the diagnostic identifies the missing project object") {
        require_diagnostic(errors, ManifestErrorKind::field, "project",
                           "project");
      }
    }
  }
}

SCENARIO("A missing project name is rejected", "[manifest]") {
  GIVEN("a manifest without a project name") {
    auto const manifest = read_manifest(
        std::filesystem::path(CXXP_TEST_FIXTURES) / "missing-name.json");
    WHEN("the manifest is validated") {
      auto const errors = validate_manifest(manifest);
      THEN("the diagnostic identifies the missing project name") {
        require_diagnostic(errors, ManifestErrorKind::field, "project.name",
                           "name");
      }
    }
  }
}

SCENARIO("A missing project version is rejected", "[manifest]") {
  GIVEN("a manifest without a project version") {
    auto const manifest = read_manifest(
        std::filesystem::path(CXXP_TEST_FIXTURES) / "missing-version.json");
    WHEN("the manifest is validated") {
      auto const errors = validate_manifest(manifest);
      THEN("the diagnostic identifies the missing project version") {
        require_diagnostic(errors, ManifestErrorKind::field, "project",
                           "version");
      }
    }
  }
}

SCENARIO("The schema identifier cannot be null", "[manifest]") {
  GIVEN("a manifest whose schema identifier is null") {
    auto const manifest =
        read_manifest(std::filesystem::path(CXXP_TEST_FIXTURES) /
                      "type-schemaVersion-0.json");
    WHEN("the manifest is validated") {
      auto const errors = validate_manifest(manifest);
      THEN("the diagnostic requires an integer schema identifier") {
        require_diagnostic(errors, ManifestErrorKind::schema_identifier,
                           "schemaVersion", "integer");
      }
    }
  }
}

SCENARIO("The schema identifier cannot be a boolean", "[manifest]") {
  GIVEN("a manifest whose schema identifier is a boolean") {
    auto const manifest =
        read_manifest(std::filesystem::path(CXXP_TEST_FIXTURES) /
                      "type-schemaVersion-1.json");
    WHEN("the manifest is validated") {
      auto const errors = validate_manifest(manifest);
      THEN("the diagnostic requires an integer schema identifier") {
        require_diagnostic(errors, ManifestErrorKind::schema_identifier,
                           "schemaVersion", "integer");
      }
    }
  }
}

SCENARIO("The schema identifier cannot be an array", "[manifest]") {
  GIVEN("a manifest whose schema identifier is an array") {
    auto const manifest =
        read_manifest(std::filesystem::path(CXXP_TEST_FIXTURES) /
                      "type-schemaVersion-2.json");
    WHEN("the manifest is validated") {
      auto const errors = validate_manifest(manifest);
      THEN("the diagnostic requires an integer schema identifier") {
        require_diagnostic(errors, ManifestErrorKind::schema_identifier,
                           "schemaVersion", "integer");
      }
    }
  }
}

SCENARIO("The schema identifier cannot be an object", "[manifest]") {
  GIVEN("a manifest whose schema identifier is an object") {
    auto const manifest =
        read_manifest(std::filesystem::path(CXXP_TEST_FIXTURES) /
                      "type-schemaVersion-3.json");
    WHEN("the manifest is validated") {
      auto const errors = validate_manifest(manifest);
      THEN("the diagnostic requires an integer schema identifier") {
        require_diagnostic(errors, ManifestErrorKind::schema_identifier,
                           "schemaVersion", "integer");
      }
    }
  }
}

SCENARIO("Schema revision 7 is unsupported", "[manifest]") {
  GIVEN("a manifest requesting schema revision 7") {
    auto const manifest =
        read_manifest(std::filesystem::path(CXXP_TEST_FIXTURES) /
                      "type-schemaVersion-4.json");
    WHEN("the manifest is validated") {
      auto const errors = validate_manifest(manifest);
      THEN("the diagnostic identifies revision 1 as supported") {
        require_diagnostic(errors, ManifestErrorKind::unsupported_revision,
                           "schemaVersion", "1");
      }
    }
  }
}

SCENARIO("The schema identifier cannot be a string", "[manifest]") {
  GIVEN("a manifest whose schema identifier is a string") {
    auto const manifest =
        read_manifest(std::filesystem::path(CXXP_TEST_FIXTURES) /
                      "type-schemaVersion-5.json");
    WHEN("the manifest is validated") {
      auto const errors = validate_manifest(manifest);
      THEN("the diagnostic requires an integer schema identifier") {
        require_diagnostic(errors, ManifestErrorKind::schema_identifier,
                           "schemaVersion", "integer");
      }
    }
  }
}

SCENARIO("The project cannot be null", "[manifest]") {
  GIVEN("a manifest whose project is null") {
    auto const manifest = read_manifest(
        std::filesystem::path(CXXP_TEST_FIXTURES) / "type-project-0.json");
    WHEN("the manifest is validated") {
      auto const errors = validate_manifest(manifest);
      THEN("the diagnostic requires a project object") {
        require_diagnostic(errors, ManifestErrorKind::field, "project",
                           "object");
      }
    }
  }
}

SCENARIO("The project cannot be a boolean", "[manifest]") {
  GIVEN("a manifest whose project is a boolean") {
    auto const manifest = read_manifest(
        std::filesystem::path(CXXP_TEST_FIXTURES) / "type-project-1.json");
    WHEN("the manifest is validated") {
      auto const errors = validate_manifest(manifest);
      THEN("the diagnostic requires a project object") {
        require_diagnostic(errors, ManifestErrorKind::field, "project",
                           "object");
      }
    }
  }
}

SCENARIO("The project cannot be an array", "[manifest]") {
  GIVEN("a manifest whose project is an array") {
    auto const manifest = read_manifest(
        std::filesystem::path(CXXP_TEST_FIXTURES) / "type-project-2.json");
    WHEN("the manifest is validated") {
      auto const errors = validate_manifest(manifest);
      THEN("the diagnostic requires a project object") {
        require_diagnostic(errors, ManifestErrorKind::field, "project",
                           "object");
      }
    }
  }
}

SCENARIO("An empty project object is rejected", "[manifest]") {
  GIVEN("a manifest whose project is an empty object") {
    auto const manifest = read_manifest(
        std::filesystem::path(CXXP_TEST_FIXTURES) / "type-project-3.json");
    WHEN("the manifest is validated") {
      auto const errors = validate_manifest(manifest);
      THEN("the diagnostic identifies the missing project name") {
        require_diagnostic(errors, ManifestErrorKind::field, "project.name",
                           "name");
      }
    }
  }
}

SCENARIO("The project cannot be an integer", "[manifest]") {
  GIVEN("a manifest whose project is an integer") {
    auto const manifest = read_manifest(
        std::filesystem::path(CXXP_TEST_FIXTURES) / "type-project-4.json");
    WHEN("the manifest is validated") {
      auto const errors = validate_manifest(manifest);
      THEN("the diagnostic requires a project object") {
        require_diagnostic(errors, ManifestErrorKind::field, "project",
                           "object");
      }
    }
  }
}

SCENARIO("The project cannot be a string", "[manifest]") {
  GIVEN("a manifest whose project is a string") {
    auto const manifest = read_manifest(
        std::filesystem::path(CXXP_TEST_FIXTURES) / "type-project-5.json");
    WHEN("the manifest is validated") {
      auto const errors = validate_manifest(manifest);
      THEN("the diagnostic requires a project object") {
        require_diagnostic(errors, ManifestErrorKind::field, "project",
                           "object");
      }
    }
  }
}

SCENARIO("The project name cannot be null", "[manifest]") {
  GIVEN("a manifest whose project name is null") {
    auto const manifest = read_manifest(
        std::filesystem::path(CXXP_TEST_FIXTURES) / "type-name-0.json");
    WHEN("the manifest is validated") {
      auto const errors = validate_manifest(manifest);
      THEN("the diagnostic requires a string project name") {
        require_diagnostic(errors, ManifestErrorKind::field, "project.name",
                           "string");
      }
    }
  }
}

SCENARIO("The project name cannot be a boolean", "[manifest]") {
  GIVEN("a manifest whose project name is a boolean") {
    auto const manifest = read_manifest(
        std::filesystem::path(CXXP_TEST_FIXTURES) / "type-name-1.json");
    WHEN("the manifest is validated") {
      auto const errors = validate_manifest(manifest);
      THEN("the diagnostic requires a string project name") {
        require_diagnostic(errors, ManifestErrorKind::field, "project.name",
                           "string");
      }
    }
  }
}

SCENARIO("The project name cannot be an array", "[manifest]") {
  GIVEN("a manifest whose project name is an array") {
    auto const manifest = read_manifest(
        std::filesystem::path(CXXP_TEST_FIXTURES) / "type-name-2.json");
    WHEN("the manifest is validated") {
      auto const errors = validate_manifest(manifest);
      THEN("the diagnostic requires a string project name") {
        require_diagnostic(errors, ManifestErrorKind::field, "project.name",
                           "string");
      }
    }
  }
}

SCENARIO("The project name cannot be an object", "[manifest]") {
  GIVEN("a manifest whose project name is an object") {
    auto const manifest = read_manifest(
        std::filesystem::path(CXXP_TEST_FIXTURES) / "type-name-3.json");
    WHEN("the manifest is validated") {
      auto const errors = validate_manifest(manifest);
      THEN("the diagnostic requires a string project name") {
        require_diagnostic(errors, ManifestErrorKind::field, "project.name",
                           "string");
      }
    }
  }
}

SCENARIO("The project name cannot be an integer", "[manifest]") {
  GIVEN("a manifest whose project name is an integer") {
    auto const manifest = read_manifest(
        std::filesystem::path(CXXP_TEST_FIXTURES) / "type-name-4.json");
    WHEN("the manifest is validated") {
      auto const errors = validate_manifest(manifest);
      THEN("the diagnostic requires a string project name") {
        require_diagnostic(errors, ManifestErrorKind::field, "project.name",
                           "string");
      }
    }
  }
}

SCENARIO("The project version cannot be null", "[manifest]") {
  GIVEN("a manifest whose project version is null") {
    auto const manifest = read_manifest(
        std::filesystem::path(CXXP_TEST_FIXTURES) / "type-version-0.json");
    WHEN("the manifest is validated") {
      auto const errors = validate_manifest(manifest);
      THEN("the diagnostic requires a string project version") {
        require_diagnostic(errors, ManifestErrorKind::field, "project.version",
                           "string");
      }
    }
  }
}

SCENARIO("The project version cannot be a boolean", "[manifest]") {
  GIVEN("a manifest whose project version is a boolean") {
    auto const manifest = read_manifest(
        std::filesystem::path(CXXP_TEST_FIXTURES) / "type-version-1.json");
    WHEN("the manifest is validated") {
      auto const errors = validate_manifest(manifest);
      THEN("the diagnostic requires a string project version") {
        require_diagnostic(errors, ManifestErrorKind::field, "project.version",
                           "string");
      }
    }
  }
}

SCENARIO("The project version cannot be an array", "[manifest]") {
  GIVEN("a manifest whose project version is an array") {
    auto const manifest = read_manifest(
        std::filesystem::path(CXXP_TEST_FIXTURES) / "type-version-2.json");
    WHEN("the manifest is validated") {
      auto const errors = validate_manifest(manifest);
      THEN("the diagnostic requires a string project version") {
        require_diagnostic(errors, ManifestErrorKind::field, "project.version",
                           "string");
      }
    }
  }
}

SCENARIO("The project version cannot be an object", "[manifest]") {
  GIVEN("a manifest whose project version is an object") {
    auto const manifest = read_manifest(
        std::filesystem::path(CXXP_TEST_FIXTURES) / "type-version-3.json");
    WHEN("the manifest is validated") {
      auto const errors = validate_manifest(manifest);
      THEN("the diagnostic requires a string project version") {
        require_diagnostic(errors, ManifestErrorKind::field, "project.version",
                           "string");
      }
    }
  }
}

SCENARIO("The project version cannot be an integer", "[manifest]") {
  GIVEN("a manifest whose project version is an integer") {
    auto const manifest = read_manifest(
        std::filesystem::path(CXXP_TEST_FIXTURES) / "type-version-4.json");
    WHEN("the manifest is validated") {
      auto const errors = validate_manifest(manifest);
      THEN("the diagnostic requires a string project version") {
        require_diagnostic(errors, ManifestErrorKind::field, "project.version",
                           "string");
      }
    }
  }
}

SCENARIO("An empty project name is rejected", "[manifest]") {
  GIVEN("a manifest whose project name is an empty string") {
    auto const manifest = read_manifest(
        std::filesystem::path(CXXP_TEST_FIXTURES) / "empty-name.json");
    WHEN("the manifest is validated") {
      auto const errors = validate_manifest(manifest);
      THEN("the diagnostic explains the project name length constraint") {
        require_diagnostic(errors, ManifestErrorKind::field, "project.name",
                           "length");
      }
    }
  }
}

SCENARIO("An empty project version is rejected", "[manifest]") {
  GIVEN("a manifest whose project version is an empty string") {
    auto const manifest = read_manifest(
        std::filesystem::path(CXXP_TEST_FIXTURES) / "empty-version.json");
    WHEN("the manifest is validated") {
      auto const errors = validate_manifest(manifest);
      THEN("the diagnostic explains the project version length constraint") {
        require_diagnostic(errors, ManifestErrorKind::field, "project.version",
                           "length");
      }
    }
  }
}

SCENARIO("Unknown fields in the root are rejected", "[manifest]") {
  GIVEN("a manifest with an extra field named typo in the root") {
    auto const manifest = read_manifest(
        std::filesystem::path(CXXP_TEST_FIXTURES) / "unknown-root.json");
    WHEN("the manifest is validated") {
      auto const errors = validate_manifest(manifest);
      THEN("the diagnostic identifies typo as a field that is not allowed") {
        require_diagnostic(errors, ManifestErrorKind::field, "typo",
                           "not allowed");
      }
    }
  }
}

SCENARIO("Unknown fields in the project object are rejected", "[manifest]") {
  GIVEN("a manifest with an extra field named typo in the project object") {
    auto const manifest = read_manifest(
        std::filesystem::path(CXXP_TEST_FIXTURES) / "unknown-project.json");
    WHEN("the manifest is validated") {
      auto const errors = validate_manifest(manifest);
      THEN("the diagnostic identifies typo as a field that is not allowed") {
        require_diagnostic(errors, ManifestErrorKind::field, "typo",
                           "not allowed");
      }
    }
  }
}

SCENARIO("A manifest cannot be null", "[manifest]") {
  GIVEN("a JSON document whose root is null") {
    auto const manifest = read_manifest(
        std::filesystem::path(CXXP_TEST_FIXTURES) / "root-0.json");
    WHEN("the manifest is validated") {
      auto const errors = validate_manifest(manifest);
      THEN("the diagnostic requires an object at the document root") {
        require_diagnostic(errors, ManifestErrorKind::field, "$", "object");
      }
    }
  }
}

SCENARIO("A manifest cannot be a boolean", "[manifest]") {
  GIVEN("a JSON document whose root is a boolean") {
    auto const manifest = read_manifest(
        std::filesystem::path(CXXP_TEST_FIXTURES) / "root-1.json");
    WHEN("the manifest is validated") {
      auto const errors = validate_manifest(manifest);
      THEN("the diagnostic requires an object at the document root") {
        require_diagnostic(errors, ManifestErrorKind::field, "$", "object");
      }
    }
  }
}

SCENARIO("A manifest cannot be an array", "[manifest]") {
  GIVEN("a JSON document whose root is an array") {
    auto const manifest = read_manifest(
        std::filesystem::path(CXXP_TEST_FIXTURES) / "root-2.json");
    WHEN("the manifest is validated") {
      auto const errors = validate_manifest(manifest);
      THEN("the diagnostic requires an object at the document root") {
        require_diagnostic(errors, ManifestErrorKind::field, "$", "object");
      }
    }
  }
}

SCENARIO("A manifest cannot be an integer", "[manifest]") {
  GIVEN("a JSON document whose root is an integer") {
    auto const manifest = read_manifest(
        std::filesystem::path(CXXP_TEST_FIXTURES) / "root-3.json");
    WHEN("the manifest is validated") {
      auto const errors = validate_manifest(manifest);
      THEN("the diagnostic requires an object at the document root") {
        require_diagnostic(errors, ManifestErrorKind::field, "$", "object");
      }
    }
  }
}

SCENARIO("A manifest cannot be a string", "[manifest]") {
  GIVEN("a JSON document whose root is a string") {
    auto const manifest = read_manifest(
        std::filesystem::path(CXXP_TEST_FIXTURES) / "root-4.json");
    WHEN("the manifest is validated") {
      auto const errors = validate_manifest(manifest);
      THEN("the diagnostic requires an object at the document root") {
        require_diagnostic(errors, ManifestErrorKind::field, "$", "object");
      }
    }
  }
}

SCENARIO("Schema revision 0 is unsupported", "[manifest]") {
  GIVEN("a manifest requesting schema revision 0") {
    auto const manifest = read_manifest(
        std::filesystem::path(CXXP_TEST_FIXTURES) / "unsupported-0.json");
    WHEN("the manifest is validated") {
      auto const errors = validate_manifest(manifest);
      THEN("the diagnostic identifies revision 1 as supported") {
        require_diagnostic(errors, ManifestErrorKind::unsupported_revision,
                           "schemaVersion", "1");
      }
    }
  }
}

SCENARIO("Schema revision -1 is unsupported", "[manifest]") {
  GIVEN("a manifest requesting schema revision -1") {
    auto const manifest = read_manifest(
        std::filesystem::path(CXXP_TEST_FIXTURES) / "unsupported-1.json");
    WHEN("the manifest is validated") {
      auto const errors = validate_manifest(manifest);
      THEN("the diagnostic identifies revision 1 as supported") {
        require_diagnostic(errors, ManifestErrorKind::unsupported_revision,
                           "schemaVersion", "1");
      }
    }
  }
}

SCENARIO("Schema revision 2 is unsupported", "[manifest]") {
  GIVEN("a manifest requesting schema revision 2") {
    auto const manifest = read_manifest(
        std::filesystem::path(CXXP_TEST_FIXTURES) / "unsupported-2.json");
    WHEN("the manifest is validated") {
      auto const errors = validate_manifest(manifest);
      THEN("the diagnostic identifies revision 1 as supported") {
        require_diagnostic(errors, ManifestErrorKind::unsupported_revision,
                           "schemaVersion", "1");
      }
    }
  }
}

SCENARIO("Schema revision 18446744073709551615 is unsupported", "[manifest]") {
  GIVEN("a manifest requesting schema revision 18446744073709551615") {
    auto const manifest = read_manifest(
        std::filesystem::path(CXXP_TEST_FIXTURES) / "unsupported-3.json");
    WHEN("the manifest is validated") {
      auto const errors = validate_manifest(manifest);
      THEN("the diagnostic identifies revision 1 as supported") {
        require_diagnostic(errors, ManifestErrorKind::unsupported_revision,
                           "schemaVersion", "1");
      }
    }
  }
}

SCENARIO("A floating-point schema identifier is rejected even when numerically "
         "equal to 1",
         "[manifest]") {
  GIVEN("a manifest whose schema identifier is 1.0") {
    auto const manifest = read_manifest(
        std::filesystem::path(CXXP_TEST_FIXTURES) / "float-identifier.json");
    WHEN("the manifest is validated") {
      auto const errors = validate_manifest(manifest);
      THEN("the diagnostic requires an integer schema identifier") {
        require_diagnostic(errors, ManifestErrorKind::schema_identifier,
                           "schemaVersion", "integer");
      }
    }
  }
}

SCENARIO("The repository manifest matches the project", "[manifest]") {
  GIVEN("the repository's cxx.json") {
    auto const manifest = read_manifest(CXXP_TEST_REPOSITORY_MANIFEST);
    WHEN("the manifest is validated") {
      auto const errors = validate_manifest(manifest);
      THEN("it is valid") { REQUIRE(errors.empty()); }
      AND_THEN("its version matches the CMake project version") {
        REQUIRE(manifest.at("project").at("version").get<std::string>() ==
                std::string_view(CXXP_TEST_PROJECT_VERSION));
      }
    }
  }
}
} // namespace cxxp
