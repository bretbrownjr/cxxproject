#include <cxxp/format_project.hxx>
#include <cxxp/format_runner.hxx>
#include <cxxp/lock_file.hxx>
#include <cxxp/process.hxx>
#include <cxxp/project.hxx>

#include <stdexcept>

namespace cxxp {
namespace {

Project discover(std::filesystem::path const &start) {
  try {
    return load_project(start);
  } catch (std::exception const &error) {
    throw std::runtime_error(std::string(error.what()) +
                             "\nCorrect the project declaration or discovery "
                             "error, then run cxxp lock and retry.");
  }
}

std::filesystem::path prepare_lock(Project const &project, bool locked,
                                   bool update_tool) {
  auto const path = project.root / "cxx.lock.json";
  auto lock = [&] {
    try {
      return LockFile::load(path).value_or(LockFile{});
    } catch (std::exception const &error) {
      throw std::runtime_error(
          path.string() + ": " + error.what() +
          "\nCorrect or restore the lockfile, then run cxxp lock and retry.");
    }
  }();
  auto const expected = lock.version(ClangFormatBackend::tool_id);
  if (locked && !expected) {
    throw std::runtime_error(
        path.string() +
        ": missing required clang-format record; run cxxp lock, review its "
        "changes, then retry cxxp format --check --locked.");
  }

  std::filesystem::path executable;
  ToolIdentity identity;
  try {
    executable = resolve_executable(ClangFormatBackend::tool_id);
    auto result =
        run_process(executable, ClangFormatBackend::identification_arguments(),
                    project.root, true);
    if (!result.succeeded()) {
      throw std::runtime_error(executable.string() +
                               (result.signal
                                    ? ": version query terminated by signal " +
                                          std::to_string(result.signal)
                                    : ": version query exited with status " +
                                          std::to_string(result.exit_code)));
    }
    identity = ClangFormatBackend::identify(result.output);
  } catch (std::exception const &error) {
    throw std::runtime_error(
        std::string("clang-format: ") + error.what() +
        "\nInstall or select clang-format " + expected.value_or("22.1.8") +
        " on PATH, then run cxxp lock and retry formatting.");
  }
  if (expected && !update_tool && *expected != identity.version) {
    throw std::runtime_error(
        executable.string() + ": clang-format expected " + *expected +
        ", detected " + identity.version +
        ".\nSelect the locked release on PATH and retry, or deliberately run "
        "cxxp lock --update-tool clang-format followed by cxxp format.");
  }
  if (!ClangFormatBackend::supports(identity.version)) {
    throw std::runtime_error(
        executable.string() + ": unsupported clang-format release " +
        identity.version +
        "; this backend supports 22.1.8.\nSelect 22.1.8 on PATH, then run cxxp "
        "lock" +
        (expected && *expected != "22.1.8" ? " --update-tool clang-format"
                                           : "") +
        " and retry formatting.");
  }
  if (!expected || (update_tool && *expected != identity.version)) {
    lock.set_tool(ClangFormatBackend::tool_id, identity.version);
    lock.write_atomic(path);
  }
  return executable;
}

} // unnamed namespace

void format_project(std::filesystem::path const &start,
                    FormatOperation operation, bool locked) {
  auto const project = discover(start);
  std::vector<std::filesystem::path> files;
  for (auto const &source : project.sources) {
    files.push_back(project.root / source);
  }
  for (auto const &header : project.headers) {
    files.push_back(project.root / header);
  }
  auto const executable = prepare_lock(project, locked, false);
  run_formatter(executable, project.root, std::move(files), operation);
}

void lock_project(std::filesystem::path const &start, bool update_tool) {
  prepare_lock(discover(start), false, update_tool);
}

} // namespace cxxp
