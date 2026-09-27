#include <cxxp/project.hxx>
#include <cxxp/project_info.hxx>
#include <cxxp/test/test_support.hxx>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include <filesystem>
#include <system_error>
#include <vector>

namespace cxxp {
namespace {
constexpr auto valid_project_file = R"({
  "schemaVersion": 1,
  "project": {"name": "sample", "version": "0.1"}
})";
} // unnamed namespace

SCENARIO("Project loading selects and validates the nearest cxx.json file",
         "[project]") {
  GIVEN("a valid project containing a nested project with an invalid cxx.json "
        "file") {
    test::TemporaryProject fixture;
    fixture.write(test::NamedFile{"cxx.json", valid_project_file},
                  test::NamedFile{"nested/cxx.json", "{\"schemaVersion\": 2}"});
    WHEN("the project is loaded from the nested directory") {
      THEN("the invalid nearest cxx.json file is reported") {
        REQUIRE_THROWS_WITH(load_project(fixture.root() / "nested"),
                            Catch::Matchers::ContainsSubstring(
                                (fixture.root() / "nested/cxx.json").string()));
      }
    }
  }
}

SCENARIO("Project discovery sorts files and skips excluded trees",
         "[project]") {
  GIVEN(
      "a project with source files, configurations, and excluded directories") {
    test::TemporaryProject fixture;
    fixture.write(test::NamedFile{"cxx.json", valid_project_file},
                  test::NamedFile{"nested/cxx.json", "invalid"});
    fixture.touch("CMakeLists.txt", "src/z.cxx", "src/a.cpp", "src/ignored.CXX",
                  "include/sample.hxx", ".clang-format", "tools/_clang-format",
                  "tools/.clang-tidy", "build/generated.cxx",
                  ".git/ignored.cxx", ".hg/ignored.cxx", ".svn/ignored.cxx",
                  "cmake-build-debug/ignored.cxx", "cached/CMakeCache.txt",
                  "cached/ignored.cxx", "nested/ignored.hxx");
    WHEN("the project is loaded") {
      auto const project = load_project(fixture.root());
      THEN("matching files are sorted and excluded entries are absent") {
        REQUIRE(project.sources ==
                std::vector<std::string>{"src/a.cpp", "src/z.cxx"});
        REQUIRE(project.headers ==
                std::vector<std::string>{"include/sample.hxx"});
        REQUIRE(
            project.clang_format_configurations ==
            std::vector<std::string>{".clang-format", "tools/_clang-format"});
        REQUIRE(project.clang_tidy_configurations ==
                std::vector<std::string>{"tools/.clang-tidy"});
        REQUIRE(project.backend == BuildBackend::cmake);
      }
    }
  }
}

SCENARIO("Project discovery skips directory symlinks", "[project]") {
  GIVEN("a project with a directory symlink to source files") {
    test::TemporaryProject fixture;
    fixture.write(test::NamedFile{"cxx.json", valid_project_file});
    fixture.touch("src/example.cxx");
    std::error_code symlink_error;
    std::filesystem::create_directory_symlink(
        fixture.root() / "src", fixture.root() / "linked-src", symlink_error);
    if (symlink_error) {
      SKIP("directory symlinks are unavailable: " + symlink_error.message());
    }

    WHEN("the project is loaded") {
      auto const project = load_project(fixture.root());
      THEN("the linked source is not added") {
        REQUIRE(project.sources == std::vector<std::string>{"src/example.cxx"});
      }
    }
  }
}

SCENARIO("Project loading does not skip a dangling nearest cxx.json file",
         "[project]") {
  GIVEN("a valid parent project and a dangling cxx.json symlink in a child") {
    test::TemporaryProject fixture;
    fixture.write(test::NamedFile{"cxx.json", valid_project_file});
    fixture.touch("nested/placeholder.txt");
    auto const nested_project_file = fixture.root() / "nested/cxx.json";
    std::error_code symlink_error;
    std::filesystem::create_symlink("missing.json", nested_project_file,
                                    symlink_error);
    if (symlink_error) {
      SKIP("file symlinks are unavailable: " + symlink_error.message());
    }

    WHEN("the child project is loaded") {
      THEN("the dangling cxx.json file is reported") {
        REQUIRE_THROWS_WITH(
            load_project(fixture.root() / "nested"),
            Catch::Matchers::ContainsSubstring(nested_project_file.string()));
      }
    }
  }
}

SCENARIO("Duplicate JSON object keys fail before cxx.json validation",
         "[project]") {
  GIVEN("a cxx.json file with a duplicate schemaVersion key") {
    test::TemporaryProject fixture;
    fixture.write(test::NamedFile{"cxx.json",
                                  R"({"schemaVersion":1,"schemaVersion":1})"});
    WHEN("the project is loaded") {
      THEN("loading reports an error") {
        REQUIRE_THROWS(load_project(fixture.root()));
      }
    }
  }
}

SCENARIO("Malformed JSON is reported as a project loading error", "[project]") {
  GIVEN("a cxx.json file containing incomplete JSON") {
    test::TemporaryProject fixture;
    fixture.write(test::NamedFile{"cxx.json", "{"});
    WHEN("the project is loaded") {
      THEN("loading reports an error") {
        REQUIRE_THROWS(load_project(fixture.root()));
      }
    }
  }
}

SCENARIO("Project information includes empty inventory and a null backend",
         "[project]") {
  GIVEN("a valid project without source files or a CMake entry point") {
    test::TemporaryProject fixture;
    fixture.write(test::NamedFile{"cxx.json", valid_project_file});
    WHEN("the project is converted to info output") {
      auto const project = load_project(fixture.root());
      nlohmann::ordered_json const info = project;
      THEN("the file arrays are empty and the build backend is null") {
        REQUIRE(info.at("projectFile") == "cxx.json");
        REQUIRE(project.backend == BuildBackend::none);
        REQUIRE(info.at("files").at("sources").empty());
        REQUIRE(info.at("files").at("headers").empty());
        REQUIRE(info.at("build").at("backend").is_null());
        REQUIRE(info.at("build").at("entryPoint").is_null());
      }
    }
  }
}
} // namespace cxxp
