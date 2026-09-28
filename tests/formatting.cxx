#include <catch2/catch_test_macros.hpp>
#include <cxxp/format_backend.hxx>
#include <cxxp/lock_file.hxx>
#include <cxxp/project.hxx>
#include <cxxp/project_file.hxx>
#include <cxxp/test/test_support.hxx>

#include <array>
#include <fstream>

namespace cxxp {

SCENARIO("Formatting declarations remain optional and strictly validated") {
  nlohmann::json doc = {{"schemaVersion", 1},
                        {"project", {{"name", "test"}, {"version", "1"}}}};

  GIVEN("an existing manifest") {
    REQUIRE(validate_project_file(doc).empty());

    for (auto const &value : {nlohmann::json::object(),
                              nlohmann::json{{"backend", "clang-format"}}}) {
      doc["formatting"] = value;

      REQUIRE(validate_project_file(doc).empty());
    }

    for (auto const &value :
         {nlohmann::json(nullptr), nlohmann::json("clang-format"),
          nlohmann::json{{"backend", "other"}}, nlohmann::json{{"backend", 1}},
          nlohmann::json{{"typo", true}}}) {
      doc["formatting"] = value;

      REQUIRE_FALSE(validate_project_file(doc).empty());
    }
  }

  GIVEN("a project with formatting declared and a malformed lock") {
    test::TemporaryProject project;
    doc["formatting"] = {{"backend", "clang-format"}};
    project.write(test::NamedFile{"cxx.json", doc.dump()},
                  test::NamedFile{"cxx.lock.json", "invalid"});

    THEN("loading only models the declaration") {
      REQUIRE(load_project(project.root()).formatting_backend ==
              FormattingBackend::clang_format);
    }
  }
}

SCENARIO("Lock validation rejects invalid metadata without replacing it") {
  test::TemporaryProject project;
  auto path = project.root() / "cxx.lock.json";

  REQUIRE_FALSE(LockFile::load(path));

  for (
      auto text :
      {"{", R"({"schemaVersion":1,"schemaVersion":1})",
       R"({"schemaVersion":1,"projectDependencies":{"a":{"x":1,"x":2}},"developmentTools":{}})"}) {
    project.write({"cxx.lock.json", text});

    REQUIRE_THROWS(LockFile::load(path));
  }

  auto base = LockFile().document();

  for (auto version : {nlohmann::json(2), nlohmann::json(1.0),
                       nlohmann::json("1"), nlohmann::json(nullptr)}) {
    auto doc = base;
    doc["schemaVersion"] = version;

    REQUIRE_THROWS(LockFile(doc));
  }

  for (auto key :
       {"schemaVersion", "projectDependencies", "developmentTools"}) {
    auto doc = base;
    doc.erase(key);

    REQUIRE_THROWS(LockFile(doc));
  }

  for (auto record :
       {nlohmann::json::object(), nlohmann::json{{"version", ""}},
        nlohmann::json{{"version", 1}}, nlohmann::json{{"version", "22.1"}},
        nlohmann::json{{"version", "22.1.8"}, {"path", "/bin/tool"}}}) {
    auto doc = base;
    doc["developmentTools"]["clang-format"] = record;

    REQUIRE_THROWS(LockFile(doc));
  }

  base["unknown"] = true;

  REQUIRE_THROWS(LockFile(base));
}

SCENARIO("Lock updates preserve unrelated records and write atomically") {
  test::TemporaryProject project;
  auto path = project.root() / "cxx.lock.json";
  auto doc = LockFile().document();
  doc["projectDependencies"] = {{"library", {{"arbitrary", {1, 2, 3}}}}};
  doc["developmentTools"]["other"] = {{"version", "opaque"}};

  LockFile lock(doc);
  std::array<std::string_view, 1> required{"clang-format"};

  REQUIRE_THROWS(lock.require_tools(required));

  lock.set_tool("clang-format", "22.1.8");

  REQUIRE_NOTHROW(lock.require_tools(required));
  REQUIRE_THROWS(lock.set_tool("clang-format", "bad"));

  lock.write_atomic(path);
  auto loaded = LockFile::load(path);

  REQUIRE(loaded->version("clang-format") == "22.1.8");
  REQUIRE(loaded->document()["projectDependencies"] ==
          doc["projectDependencies"]);
  REQUIRE(loaded->document()["developmentTools"]["other"] ==
          doc["developmentTools"]["other"]);

  lock.set_tool("clang-format", "23.0.0");
  lock.write_atomic(path);

  REQUIRE(LockFile::load(path)->version("clang-format") == "23.0.0");

  WHEN("the destination is a directory") {
    auto directory = project.root() / "blocked";
    std::filesystem::create_directory(directory);
    project.write({"blocked/keep", "old data"});

    REQUIRE_THROWS(lock.write_atomic(directory));
    REQUIRE(std::filesystem::exists(directory / "keep"));

    for (auto const &entry :
         std::filesystem::directory_iterator(project.root())) {
      REQUIRE(entry.path().filename().string().find(".tmp.") ==
              std::string::npos);
    }
  }
}

SCENARIO(
    "The clang-format adapter separates identity and support from invocation") {
  auto identity = ClangFormatBackend::identify(
      "Ubuntu clang-format version 22.1.8 (vendor build)\n");

  REQUIRE(identity.version == "22.1.8");
  REQUIRE(identity.diagnostic_output.find("vendor") != std::string::npos);
  REQUIRE(ClangFormatBackend::supports(identity.version));
  REQUIRE_FALSE(ClangFormatBackend::supports("23.0.0"));

  for (auto output :
       {"22.1.8", "clang-format version 22.1", "clang-format version 22.1.8git",
        "clang-format version 22.1.8\nclang-format version 23.0.0"}) {
    REQUIRE_THROWS(ClangFormatBackend::identify(output));
  }

  auto file = std::filesystem::path("/tmp/@odd name;.cxx");

  REQUIRE(ClangFormatBackend::arguments(FormatOperation::rewrite, file) ==
          std::vector<std::string>{"--style=file",
                                   "--fail-on-incomplete-format", "-i",
                                   file.string()});

  REQUIRE(ClangFormatBackend::arguments(FormatOperation::check, file) ==
          std::vector<std::string>{"--style=file",
                                   "--fail-on-incomplete-format", "--dry-run",
                                   "--Werror", file.string()});

  REQUIRE_THROWS(
      ClangFormatBackend::arguments(FormatOperation::check, "relative.cxx"));
}

} // namespace cxxp
