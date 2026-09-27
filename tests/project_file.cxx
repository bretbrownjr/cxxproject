#include <cxxp/project_file.hxx>

#include <catch2/catch_test_macros.hpp>

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <string>
#include <string_view>
#include <vector>

namespace cxxp {
namespace {
nlohmann::json read_project_file(std::filesystem::path const &path) {
  INFO("Project file path: " << path);
  std::ifstream input(path);
  REQUIRE(input.is_open());
  return nlohmann::json::parse(input);
}

void require_diagnostic(std::vector<ProjectFileError> const &errors,
                        ProjectFileErrorKind kind, std::string_view field,
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

SCENARIO("A complete cxx.json file is accepted", "[project-file]") {
  GIVEN(
      "a cxx.json file with schema revision 1 and a project name and version") {
    auto const project_file = read_project_file(
        std::filesystem::path(CXXP_TEST_FIXTURES) / "valid.json");
    WHEN("the cxx.json file is validated") {
      auto const errors = validate_project_file(project_file);
      THEN("no validation errors are reported") { REQUIRE(errors.empty()); }
    }
  }
}

SCENARIO("Project versions may be arbitrary nonempty strings",
         "[project-file]") {
  GIVEN("a project version containing words, spaces, and a slash") {
    auto const project_file = read_project_file(
        std::filesystem::path(CXXP_TEST_FIXTURES) / "arbitrary-version.json");
    WHEN("the cxx.json file is validated") {
      auto const errors = validate_project_file(project_file);
      THEN("the version is accepted without imposing a versioning scheme") {
        REQUIRE(errors.empty());
      }
    }
  }
}

SCENARIO("A missing schema identifier is rejected", "[project-file]") {
  GIVEN("a cxx.json file without a schema identifier") {
    auto const project_file =
        read_project_file(std::filesystem::path(CXXP_TEST_FIXTURES) /
                          "missing-schemaVersion.json");
    WHEN("the cxx.json file is validated") {
      auto const errors = validate_project_file(project_file);
      THEN("the diagnostic identifies the missing schema identifier") {
        require_diagnostic(errors, ProjectFileErrorKind::schema_identifier,
                           "schemaVersion", "schemaVersion");
      }
    }
  }
}

SCENARIO("A missing project object is rejected", "[project-file]") {
  GIVEN("a cxx.json file without a project object") {
    auto const project_file = read_project_file(
        std::filesystem::path(CXXP_TEST_FIXTURES) / "missing-project.json");
    WHEN("the cxx.json file is validated") {
      auto const errors = validate_project_file(project_file);
      THEN("the diagnostic identifies the missing project object") {
        require_diagnostic(errors, ProjectFileErrorKind::field, "project",
                           "project");
      }
    }
  }
}

SCENARIO("A missing project name is rejected", "[project-file]") {
  GIVEN("a cxx.json file without a project name") {
    auto const project_file = read_project_file(
        std::filesystem::path(CXXP_TEST_FIXTURES) / "missing-name.json");
    WHEN("the cxx.json file is validated") {
      auto const errors = validate_project_file(project_file);
      THEN("the diagnostic identifies the missing project name") {
        require_diagnostic(errors, ProjectFileErrorKind::field, "project.name",
                           "name");
      }
    }
  }
}

SCENARIO("A missing project version is rejected", "[project-file]") {
  GIVEN("a cxx.json file without a project version") {
    auto const project_file = read_project_file(
        std::filesystem::path(CXXP_TEST_FIXTURES) / "missing-version.json");
    WHEN("the cxx.json file is validated") {
      auto const errors = validate_project_file(project_file);
      THEN("the diagnostic identifies the missing project version") {
        require_diagnostic(errors, ProjectFileErrorKind::field, "project",
                           "version");
      }
    }
  }
}

SCENARIO("The schema identifier cannot be null", "[project-file]") {
  GIVEN("a cxx.json file whose schema identifier is null") {
    auto const project_file =
        read_project_file(std::filesystem::path(CXXP_TEST_FIXTURES) /
                          "type-schemaVersion-0.json");
    WHEN("the cxx.json file is validated") {
      auto const errors = validate_project_file(project_file);
      THEN("the diagnostic requires an integer schema identifier") {
        require_diagnostic(errors, ProjectFileErrorKind::schema_identifier,
                           "schemaVersion", "integer");
      }
    }
  }
}

SCENARIO("The schema identifier cannot be a boolean", "[project-file]") {
  GIVEN("a cxx.json file whose schema identifier is a boolean") {
    auto const project_file =
        read_project_file(std::filesystem::path(CXXP_TEST_FIXTURES) /
                          "type-schemaVersion-1.json");
    WHEN("the cxx.json file is validated") {
      auto const errors = validate_project_file(project_file);
      THEN("the diagnostic requires an integer schema identifier") {
        require_diagnostic(errors, ProjectFileErrorKind::schema_identifier,
                           "schemaVersion", "integer");
      }
    }
  }
}

SCENARIO("The schema identifier cannot be an array", "[project-file]") {
  GIVEN("a cxx.json file whose schema identifier is an array") {
    auto const project_file =
        read_project_file(std::filesystem::path(CXXP_TEST_FIXTURES) /
                          "type-schemaVersion-2.json");
    WHEN("the cxx.json file is validated") {
      auto const errors = validate_project_file(project_file);
      THEN("the diagnostic requires an integer schema identifier") {
        require_diagnostic(errors, ProjectFileErrorKind::schema_identifier,
                           "schemaVersion", "integer");
      }
    }
  }
}

SCENARIO("The schema identifier cannot be an object", "[project-file]") {
  GIVEN("a cxx.json file whose schema identifier is an object") {
    auto const project_file =
        read_project_file(std::filesystem::path(CXXP_TEST_FIXTURES) /
                          "type-schemaVersion-3.json");
    WHEN("the cxx.json file is validated") {
      auto const errors = validate_project_file(project_file);
      THEN("the diagnostic requires an integer schema identifier") {
        require_diagnostic(errors, ProjectFileErrorKind::schema_identifier,
                           "schemaVersion", "integer");
      }
    }
  }
}

SCENARIO("Schema revision 7 is unsupported", "[project-file]") {
  GIVEN("a cxx.json file requesting schema revision 7") {
    auto const project_file =
        read_project_file(std::filesystem::path(CXXP_TEST_FIXTURES) /
                          "type-schemaVersion-4.json");
    WHEN("the cxx.json file is validated") {
      auto const errors = validate_project_file(project_file);
      THEN("the diagnostic identifies revision 1 as supported") {
        require_diagnostic(errors, ProjectFileErrorKind::unsupported_revision,
                           "schemaVersion", "1");
      }
    }
  }
}

SCENARIO("The schema identifier cannot be a string", "[project-file]") {
  GIVEN("a cxx.json file whose schema identifier is a string") {
    auto const project_file =
        read_project_file(std::filesystem::path(CXXP_TEST_FIXTURES) /
                          "type-schemaVersion-5.json");
    WHEN("the cxx.json file is validated") {
      auto const errors = validate_project_file(project_file);
      THEN("the diagnostic requires an integer schema identifier") {
        require_diagnostic(errors, ProjectFileErrorKind::schema_identifier,
                           "schemaVersion", "integer");
      }
    }
  }
}

SCENARIO("The project cannot be null", "[project-file]") {
  GIVEN("a cxx.json file whose project is null") {
    auto const project_file = read_project_file(
        std::filesystem::path(CXXP_TEST_FIXTURES) / "type-project-0.json");
    WHEN("the cxx.json file is validated") {
      auto const errors = validate_project_file(project_file);
      THEN("the diagnostic requires a project object") {
        require_diagnostic(errors, ProjectFileErrorKind::field, "project",
                           "object");
      }
    }
  }
}

SCENARIO("The project cannot be a boolean", "[project-file]") {
  GIVEN("a cxx.json file whose project is a boolean") {
    auto const project_file = read_project_file(
        std::filesystem::path(CXXP_TEST_FIXTURES) / "type-project-1.json");
    WHEN("the cxx.json file is validated") {
      auto const errors = validate_project_file(project_file);
      THEN("the diagnostic requires a project object") {
        require_diagnostic(errors, ProjectFileErrorKind::field, "project",
                           "object");
      }
    }
  }
}

SCENARIO("The project cannot be an array", "[project-file]") {
  GIVEN("a cxx.json file whose project is an array") {
    auto const project_file = read_project_file(
        std::filesystem::path(CXXP_TEST_FIXTURES) / "type-project-2.json");
    WHEN("the cxx.json file is validated") {
      auto const errors = validate_project_file(project_file);
      THEN("the diagnostic requires a project object") {
        require_diagnostic(errors, ProjectFileErrorKind::field, "project",
                           "object");
      }
    }
  }
}

SCENARIO("An empty project object is rejected", "[project-file]") {
  GIVEN("a cxx.json file whose project is an empty object") {
    auto const project_file = read_project_file(
        std::filesystem::path(CXXP_TEST_FIXTURES) / "type-project-3.json");
    WHEN("the cxx.json file is validated") {
      auto const errors = validate_project_file(project_file);
      THEN("the diagnostic identifies the missing project name") {
        require_diagnostic(errors, ProjectFileErrorKind::field, "project.name",
                           "name");
      }
    }
  }
}

SCENARIO("The project cannot be an integer", "[project-file]") {
  GIVEN("a cxx.json file whose project is an integer") {
    auto const project_file = read_project_file(
        std::filesystem::path(CXXP_TEST_FIXTURES) / "type-project-4.json");
    WHEN("the cxx.json file is validated") {
      auto const errors = validate_project_file(project_file);
      THEN("the diagnostic requires a project object") {
        require_diagnostic(errors, ProjectFileErrorKind::field, "project",
                           "object");
      }
    }
  }
}

SCENARIO("The project cannot be a string", "[project-file]") {
  GIVEN("a cxx.json file whose project is a string") {
    auto const project_file = read_project_file(
        std::filesystem::path(CXXP_TEST_FIXTURES) / "type-project-5.json");
    WHEN("the cxx.json file is validated") {
      auto const errors = validate_project_file(project_file);
      THEN("the diagnostic requires a project object") {
        require_diagnostic(errors, ProjectFileErrorKind::field, "project",
                           "object");
      }
    }
  }
}

SCENARIO("The project name cannot be null", "[project-file]") {
  GIVEN("a cxx.json file whose project name is null") {
    auto const project_file = read_project_file(
        std::filesystem::path(CXXP_TEST_FIXTURES) / "type-name-0.json");
    WHEN("the cxx.json file is validated") {
      auto const errors = validate_project_file(project_file);
      THEN("the diagnostic requires a string project name") {
        require_diagnostic(errors, ProjectFileErrorKind::field, "project.name",
                           "string");
      }
    }
  }
}

SCENARIO("The project name cannot be a boolean", "[project-file]") {
  GIVEN("a cxx.json file whose project name is a boolean") {
    auto const project_file = read_project_file(
        std::filesystem::path(CXXP_TEST_FIXTURES) / "type-name-1.json");
    WHEN("the cxx.json file is validated") {
      auto const errors = validate_project_file(project_file);
      THEN("the diagnostic requires a string project name") {
        require_diagnostic(errors, ProjectFileErrorKind::field, "project.name",
                           "string");
      }
    }
  }
}

SCENARIO("The project name cannot be an array", "[project-file]") {
  GIVEN("a cxx.json file whose project name is an array") {
    auto const project_file = read_project_file(
        std::filesystem::path(CXXP_TEST_FIXTURES) / "type-name-2.json");
    WHEN("the cxx.json file is validated") {
      auto const errors = validate_project_file(project_file);
      THEN("the diagnostic requires a string project name") {
        require_diagnostic(errors, ProjectFileErrorKind::field, "project.name",
                           "string");
      }
    }
  }
}

SCENARIO("The project name cannot be an object", "[project-file]") {
  GIVEN("a cxx.json file whose project name is an object") {
    auto const project_file = read_project_file(
        std::filesystem::path(CXXP_TEST_FIXTURES) / "type-name-3.json");
    WHEN("the cxx.json file is validated") {
      auto const errors = validate_project_file(project_file);
      THEN("the diagnostic requires a string project name") {
        require_diagnostic(errors, ProjectFileErrorKind::field, "project.name",
                           "string");
      }
    }
  }
}

SCENARIO("The project name cannot be an integer", "[project-file]") {
  GIVEN("a cxx.json file whose project name is an integer") {
    auto const project_file = read_project_file(
        std::filesystem::path(CXXP_TEST_FIXTURES) / "type-name-4.json");
    WHEN("the cxx.json file is validated") {
      auto const errors = validate_project_file(project_file);
      THEN("the diagnostic requires a string project name") {
        require_diagnostic(errors, ProjectFileErrorKind::field, "project.name",
                           "string");
      }
    }
  }
}

SCENARIO("The project version cannot be null", "[project-file]") {
  GIVEN("a cxx.json file whose project version is null") {
    auto const project_file = read_project_file(
        std::filesystem::path(CXXP_TEST_FIXTURES) / "type-version-0.json");
    WHEN("the cxx.json file is validated") {
      auto const errors = validate_project_file(project_file);
      THEN("the diagnostic requires a string project version") {
        require_diagnostic(errors, ProjectFileErrorKind::field,
                           "project.version", "string");
      }
    }
  }
}

SCENARIO("The project version cannot be a boolean", "[project-file]") {
  GIVEN("a cxx.json file whose project version is a boolean") {
    auto const project_file = read_project_file(
        std::filesystem::path(CXXP_TEST_FIXTURES) / "type-version-1.json");
    WHEN("the cxx.json file is validated") {
      auto const errors = validate_project_file(project_file);
      THEN("the diagnostic requires a string project version") {
        require_diagnostic(errors, ProjectFileErrorKind::field,
                           "project.version", "string");
      }
    }
  }
}

SCENARIO("The project version cannot be an array", "[project-file]") {
  GIVEN("a cxx.json file whose project version is an array") {
    auto const project_file = read_project_file(
        std::filesystem::path(CXXP_TEST_FIXTURES) / "type-version-2.json");
    WHEN("the cxx.json file is validated") {
      auto const errors = validate_project_file(project_file);
      THEN("the diagnostic requires a string project version") {
        require_diagnostic(errors, ProjectFileErrorKind::field,
                           "project.version", "string");
      }
    }
  }
}

SCENARIO("The project version cannot be an object", "[project-file]") {
  GIVEN("a cxx.json file whose project version is an object") {
    auto const project_file = read_project_file(
        std::filesystem::path(CXXP_TEST_FIXTURES) / "type-version-3.json");
    WHEN("the cxx.json file is validated") {
      auto const errors = validate_project_file(project_file);
      THEN("the diagnostic requires a string project version") {
        require_diagnostic(errors, ProjectFileErrorKind::field,
                           "project.version", "string");
      }
    }
  }
}

SCENARIO("The project version cannot be an integer", "[project-file]") {
  GIVEN("a cxx.json file whose project version is an integer") {
    auto const project_file = read_project_file(
        std::filesystem::path(CXXP_TEST_FIXTURES) / "type-version-4.json");
    WHEN("the cxx.json file is validated") {
      auto const errors = validate_project_file(project_file);
      THEN("the diagnostic requires a string project version") {
        require_diagnostic(errors, ProjectFileErrorKind::field,
                           "project.version", "string");
      }
    }
  }
}

SCENARIO("An empty project name is rejected", "[project-file]") {
  GIVEN("a cxx.json file whose project name is an empty string") {
    auto const project_file = read_project_file(
        std::filesystem::path(CXXP_TEST_FIXTURES) / "empty-name.json");
    WHEN("the cxx.json file is validated") {
      auto const errors = validate_project_file(project_file);
      THEN("the diagnostic explains the project name length constraint") {
        require_diagnostic(errors, ProjectFileErrorKind::field, "project.name",
                           "length");
      }
    }
  }
}

SCENARIO("An empty project version is rejected", "[project-file]") {
  GIVEN("a cxx.json file whose project version is an empty string") {
    auto const project_file = read_project_file(
        std::filesystem::path(CXXP_TEST_FIXTURES) / "empty-version.json");
    WHEN("the cxx.json file is validated") {
      auto const errors = validate_project_file(project_file);
      THEN("the diagnostic explains the project version length constraint") {
        require_diagnostic(errors, ProjectFileErrorKind::field,
                           "project.version", "length");
      }
    }
  }
}

SCENARIO("Unknown fields in the root are rejected", "[project-file]") {
  GIVEN("a cxx.json file with an extra field named typo in the root") {
    auto const project_file = read_project_file(
        std::filesystem::path(CXXP_TEST_FIXTURES) / "unknown-root.json");
    WHEN("the cxx.json file is validated") {
      auto const errors = validate_project_file(project_file);
      THEN("the diagnostic identifies typo as a field that is not allowed") {
        require_diagnostic(errors, ProjectFileErrorKind::field, "typo",
                           "not allowed");
      }
    }
  }
}

SCENARIO("Unknown fields in the project object are rejected",
         "[project-file]") {
  GIVEN(
      "a cxx.json file with an extra field named typo in the project object") {
    auto const project_file = read_project_file(
        std::filesystem::path(CXXP_TEST_FIXTURES) / "unknown-project.json");
    WHEN("the cxx.json file is validated") {
      auto const errors = validate_project_file(project_file);
      THEN("the diagnostic identifies typo as a field that is not allowed") {
        require_diagnostic(errors, ProjectFileErrorKind::field, "typo",
                           "not allowed");
      }
    }
  }
}

SCENARIO("A null JSON root is rejected", "[project-file]") {
  GIVEN("a JSON document whose root is null") {
    auto const project_file = read_project_file(
        std::filesystem::path(CXXP_TEST_FIXTURES) / "root-0.json");
    WHEN("the cxx.json file is validated") {
      auto const errors = validate_project_file(project_file);
      THEN("the diagnostic requires an object at the document root") {
        require_diagnostic(errors, ProjectFileErrorKind::field, "$", "object");
      }
    }
  }
}

SCENARIO("A boolean JSON root is rejected", "[project-file]") {
  GIVEN("a JSON document whose root is a boolean") {
    auto const project_file = read_project_file(
        std::filesystem::path(CXXP_TEST_FIXTURES) / "root-1.json");
    WHEN("the cxx.json file is validated") {
      auto const errors = validate_project_file(project_file);
      THEN("the diagnostic requires an object at the document root") {
        require_diagnostic(errors, ProjectFileErrorKind::field, "$", "object");
      }
    }
  }
}

SCENARIO("An array JSON root is rejected", "[project-file]") {
  GIVEN("a JSON document whose root is an array") {
    auto const project_file = read_project_file(
        std::filesystem::path(CXXP_TEST_FIXTURES) / "root-2.json");
    WHEN("the cxx.json file is validated") {
      auto const errors = validate_project_file(project_file);
      THEN("the diagnostic requires an object at the document root") {
        require_diagnostic(errors, ProjectFileErrorKind::field, "$", "object");
      }
    }
  }
}

SCENARIO("An integer JSON root is rejected", "[project-file]") {
  GIVEN("a JSON document whose root is an integer") {
    auto const project_file = read_project_file(
        std::filesystem::path(CXXP_TEST_FIXTURES) / "root-3.json");
    WHEN("the cxx.json file is validated") {
      auto const errors = validate_project_file(project_file);
      THEN("the diagnostic requires an object at the document root") {
        require_diagnostic(errors, ProjectFileErrorKind::field, "$", "object");
      }
    }
  }
}

SCENARIO("A string JSON root is rejected", "[project-file]") {
  GIVEN("a JSON document whose root is a string") {
    auto const project_file = read_project_file(
        std::filesystem::path(CXXP_TEST_FIXTURES) / "root-4.json");
    WHEN("the cxx.json file is validated") {
      auto const errors = validate_project_file(project_file);
      THEN("the diagnostic requires an object at the document root") {
        require_diagnostic(errors, ProjectFileErrorKind::field, "$", "object");
      }
    }
  }
}

SCENARIO("Schema revision 0 is unsupported", "[project-file]") {
  GIVEN("a cxx.json file requesting schema revision 0") {
    auto const project_file = read_project_file(
        std::filesystem::path(CXXP_TEST_FIXTURES) / "unsupported-0.json");
    WHEN("the cxx.json file is validated") {
      auto const errors = validate_project_file(project_file);
      THEN("the diagnostic identifies revision 1 as supported") {
        require_diagnostic(errors, ProjectFileErrorKind::unsupported_revision,
                           "schemaVersion", "1");
      }
    }
  }
}

SCENARIO("Schema revision -1 is unsupported", "[project-file]") {
  GIVEN("a cxx.json file requesting schema revision -1") {
    auto const project_file = read_project_file(
        std::filesystem::path(CXXP_TEST_FIXTURES) / "unsupported-1.json");
    WHEN("the cxx.json file is validated") {
      auto const errors = validate_project_file(project_file);
      THEN("the diagnostic identifies revision 1 as supported") {
        require_diagnostic(errors, ProjectFileErrorKind::unsupported_revision,
                           "schemaVersion", "1");
      }
    }
  }
}

SCENARIO("Schema revision 2 is unsupported", "[project-file]") {
  GIVEN("a cxx.json file requesting schema revision 2") {
    auto const project_file = read_project_file(
        std::filesystem::path(CXXP_TEST_FIXTURES) / "unsupported-2.json");
    WHEN("the cxx.json file is validated") {
      auto const errors = validate_project_file(project_file);
      THEN("the diagnostic identifies revision 1 as supported") {
        require_diagnostic(errors, ProjectFileErrorKind::unsupported_revision,
                           "schemaVersion", "1");
      }
    }
  }
}

SCENARIO("Schema revision 18446744073709551615 is unsupported",
         "[project-file]") {
  GIVEN("a cxx.json file requesting schema revision 18446744073709551615") {
    auto const project_file = read_project_file(
        std::filesystem::path(CXXP_TEST_FIXTURES) / "unsupported-3.json");
    WHEN("the cxx.json file is validated") {
      auto const errors = validate_project_file(project_file);
      THEN("the diagnostic identifies revision 1 as supported") {
        require_diagnostic(errors, ProjectFileErrorKind::unsupported_revision,
                           "schemaVersion", "1");
      }
    }
  }
}

SCENARIO("A floating-point schema identifier is rejected even when numerically "
         "equal to 1",
         "[project-file]") {
  GIVEN("a cxx.json file whose schema identifier is 1.0") {
    auto const project_file = read_project_file(
        std::filesystem::path(CXXP_TEST_FIXTURES) / "float-identifier.json");
    WHEN("the cxx.json file is validated") {
      auto const errors = validate_project_file(project_file);
      THEN("the diagnostic requires an integer schema identifier") {
        require_diagnostic(errors, ProjectFileErrorKind::schema_identifier,
                           "schemaVersion", "integer");
      }
    }
  }
}

SCENARIO("The repository cxx.json file matches the project", "[project-file]") {
  GIVEN("the repository's cxx.json") {
    auto const project_file =
        read_project_file(CXXP_TEST_REPOSITORY_PROJECT_FILE);
    WHEN("the cxx.json file is validated") {
      auto const errors = validate_project_file(project_file);
      THEN("it is valid") { REQUIRE(errors.empty()); }
      AND_THEN("its version matches the CMake project version") {
        REQUIRE(project_file.at("project").at("version").get<std::string>() ==
                std::string_view(CXXP_TEST_PROJECT_VERSION));
      }
    }
  }
}
} // namespace cxxp
