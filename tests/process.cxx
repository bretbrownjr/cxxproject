#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>
#include <csignal>
#include <cstdlib>
#include <cxxp/format_runner.hxx>
#include <cxxp/process.hxx>
#include <cxxp/test/test_support.hxx>
#include <fstream>
#include <iterator>
#include <system_error>

namespace cxxp {
namespace {

std::filesystem::path const helper = CXXP_PROCESS_HELPER;

std::string read(std::filesystem::path const &path) {
  std::ifstream stream(path);
  return {std::istreambuf_iterator<char>(stream), {}};
}

struct PathEnvironment {
  char const *original = std::getenv("PATH");
  bool existed = original != nullptr;
  std::string saved = original ? original : "";
  ~PathEnvironment() {
    if (existed)
      ::setenv("PATH", saved.c_str(), 1);
    else
      ::unsetenv("PATH");
  }
};

} // unnamed namespace

SCENARIO("Processes preserve arguments, child directory, and closed input") {
  test::TemporaryProject project;
  auto result = run_process(
      helper,
      {"inspect", "", "a b", "$(touch injected);*", "@args", "-option", "é"},
      project.root(), true);
  REQUIRE(result.succeeded());
  REQUIRE(result.output == project.root().string() +
                               "\neof\n0:\n3:a b\n19:$(touch "
                               "injected);*\n5:@args\n7:-option\n2:é\n");
  REQUIRE_FALSE(std::filesystem::exists(project.root() / "injected"));
  REQUIRE(run_process(helper, {"large"}, project.root(), true).output ==
          std::string(200000, 'x'));
  REQUIRE(run_process(helper, {"large"}, project.root()).output.empty());
}

SCENARIO("Launch errors are distinct from child exits and signals") {
  test::TemporaryProject project;
  GIVEN("a successfully launched failing child") {
    auto result = run_process(helper, {"exit"}, project.root());
    REQUIRE(result.exit_code == 37);
    REQUIRE(result.signal == 0);
    REQUIRE_FALSE(result.succeeded());
    result = run_process(helper, {"signal"}, project.root());
    REQUIRE(result.signal == SIGTERM);
    REQUIRE_FALSE(result.succeeded());
  }
  GIVEN("invalid launch inputs") {
    REQUIRE_THROWS_AS(
        run_process(project.root() / "missing", {}, project.root()),
        std::system_error);
    REQUIRE_THROWS_AS(run_process(helper, {}, project.root() / "missing"),
                      std::system_error);
    project.write({"invalid-executable", "not an executable"});
    auto invalid = project.root() / "invalid-executable";
    REQUIRE_THROWS_AS(run_process(invalid, {}, project.root()),
                      std::system_error);
    std::filesystem::permissions(invalid, std::filesystem::perms::owner_exec,
                                 std::filesystem::perm_options::add);
    REQUIRE_THROWS_AS(run_process(invalid, {}, project.root()),
                      std::system_error);
    REQUIRE_THROWS_AS(
        run_process(helper, {std::string("a\0b", 3)}, project.root()),
        std::invalid_argument);
    REQUIRE_THROWS_AS(run_process("relative", {}, project.root()),
                      std::invalid_argument);
  }
}

SCENARIO(
    "Executable selection uses the caller environment before child chdir") {
  test::TemporaryProject project;
  PathEnvironment environment;
  auto relative = std::filesystem::relative(helper.parent_path(),
                                            std::filesystem::current_path());
  REQUIRE(::setenv("PATH", relative.c_str(), 1) == 0);
  auto selected = resolve_executable(helper.filename());
  REQUIRE(selected.is_absolute());
  REQUIRE(run_process(selected, {"exit"}, project.root()).exit_code == 37);
  REQUIRE(resolve_executable(std::filesystem::relative(helper)).is_absolute());
  REQUIRE_THROWS(resolve_executable("cxxp-nonexistent-tool"));
  REQUIRE(::setenv("PATH", "", 1) == 0);
  REQUIRE_THROWS(resolve_executable(helper.filename()));
}

SCENARIO("Formatting runs sequentially in sorted order and stops on failure") {
  test::TemporaryProject project;
  auto root = project.root();
  GIVEN("an empty inventory") {
    run_formatter(helper, root, {}, FormatOperation::rewrite);
    REQUIRE_FALSE(std::filesystem::exists(root / "invocations"));
  }
  GIVEN("unsorted files including option-like names") {
    run_formatter(helper, root, {root / "z", root / "@args", root / "-option"},
                  FormatOperation::check);
    REQUIRE(read(root / "invocations") == "-option\n@args\nz\n");
  }
  GIVEN("a later file fails") {
    REQUIRE_THROWS_WITH(
        run_formatter(helper, root, {root / "z", root / "b-fail", root / "a"},
                      FormatOperation::rewrite),
        Catch::Matchers::ContainsSubstring((root / "b-fail").string() +
                                           ": exited with status 9"));
    REQUIRE(read(root / "invocations") == "a\nb-fail\n");
  }
  GIVEN("a relative path anywhere in the inventory") {
    REQUIRE_THROWS(run_formatter(helper, root, {root / "a", "z"},
                                 FormatOperation::rewrite));
    REQUIRE_FALSE(std::filesystem::exists(root / "invocations"));
  }
}

} // namespace cxxp
