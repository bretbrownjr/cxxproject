#include <cxxp/format_project.hxx>
#include <cxxp/project.hxx>
#include <cxxp/project_info.hxx>
#include <cxxp/version.hxx>

#include <exception>
#include <iostream>
#include <string_view>

namespace cxxp {
namespace {

constexpr std::string_view usage =
    "Usage: cxxp [--help | --version]\n"
    "       cxxp info [--help]\n"
    "       cxxp format [--check] [--locked]\n"
    "       cxxp lock [--update-tool clang-format]\n"
    "\n"
    "Inspect a cxxp project from its directory or a\n"
    "subdirectory.\n"
    "\n"
    "Commands:\n"
    "  info       Print project information as JSON\n"
    "  format     Format source and header files\n"
    "  lock       Record installed development tool versions\n"
    "\n"
    "Options:\n"
    "  --help     Show this help\n"
    "  --version  Show the project version\n"
    "\n"
    "Run 'cxxp info --help' for command details.\n";

constexpr std::string_view info_usage =
    "Usage: cxxp info [--help]\n\n"
    "Print a JSON summary of the project described by cxx.json. Use it to\n"
    "inspect project metadata, see which source and header files cxxp\n"
    "recognizes, or pass a stable inventory to another tool. The summary also\n"
    "lists clang-format and clang-tidy configuration files and identifies a\n"
    "CMake entry point when one is present.\n"
    "\n"
    "Project discovery starts in the current directory and walks upward to\n"
    "the nearest cxx.json. Run the command from any directory inside the\n"
    "project; paths in the output are relative to its root. Nested projects\n"
    "with their own cxx.json are reported separately.\n"
    "\n"
    "The JSON output is sorted and has no timestamps or tool versions. See\n"
    "the cxxp command reference for the complete output schema and discovery\n"
    "rules.\n";

constexpr std::string_view format_usage =
    "Usage: cxxp format [--check] [--locked]\n"
    "       cxxp format --help\n\n"
    "Format discovered sources and headers using clang-format on PATH.\n"
    "Run from any project directory; no build files are required.\n"
    "--check checks without source edits and reports all file failures.\n"
    "Missing lock records are initialized unless --locked forbids writes.\n"
    "Existing releases must match. Formatting stops at the first failed file;\n"
    "earlier edits and initialized lock metadata remain.\n";

constexpr std::string_view lock_usage =
    "Usage: cxxp lock [--update-tool clang-format]\n"
    "       cxxp lock --help\n\n"
    "Initialize missing tool records from clang-format on PATH. Existing\n"
    "selections must match. --update-tool clang-format deliberately replaces\n"
    "that selection, preserving unrelated records. No tools are installed or\n"
    "sources changed. Run cxxp format afterward to apply the selected "
    "release.\n";

} // unnamed namespace

int run(int argc, char *argv[]) {
  if (argc == 1) {
    std::cout << usage;
    return 0;
  }

  auto const command = std::string_view{argv[1]};
  if (command == "format" || command == "lock") {
    auto const help = command == "format" ? format_usage : lock_usage;
    if (argc == 3 && std::string_view{argv[2]} == "--help") {
      std::cout << help;
      return 0;
    }
    bool check = false, locked = false, update = false;
    for (int index = 2; index < argc; ++index) {
      std::string_view const argument{argv[index]};
      if (command == "format" && argument == "--check" && !check) {
        check = true;
      } else if (command == "format" && argument == "--locked" && !locked) {
        locked = true;
      } else if (command == "lock" && argument == "--update-tool" && !update &&
                 index + 1 < argc &&
                 std::string_view{argv[index + 1]} == "clang-format") {
        update = true;
        ++index;
      } else {
        std::cerr << "cxxp: unexpected argument '" << argument << "'\n" << help;
        return 1;
      }
    }
    try {
      if (command == "lock")
        lock_project(std::filesystem::current_path(), update);
      else
        format_project(
            std::filesystem::current_path(),
            check ? FormatOperation::check : FormatOperation::rewrite, locked);
      return 0;
    } catch (std::exception const &error) {
      std::cerr << "cxxp: " << error.what() << '\n';
      return 1;
    }
  }

  if (std::string_view{argv[1]} == "info") {
    if (argc == 3 && std::string_view{argv[2]} == "--help") {
      std::cout << info_usage;
      return 0;
    }
    if (argc != 2) {
      std::cerr << "cxxp: unexpected argument after 'info'\n" << info_usage;
      return 1;
    }
    try {
      auto const project = load_project();
      nlohmann::ordered_json output = project;
      std::cout << output.dump(2) << '\n';
      return 0;
    } catch (std::exception const &error) {
      std::cerr << "cxxp: " << error.what() << '\n';
      return 1;
    }
  }

  for (int index = 1; index < argc; ++index) {
    std::string_view const argument{argv[index]};
    if (argument != "--help" && argument != "--version") {
      std::cerr << "cxxp: unknown argument '" << argument << "'\n" << usage;
      return 1;
    }
  }
  if (argc != 2) {
    std::cerr << "cxxp: expected a single option\n" << usage;
    return 1;
  }
  if (std::string_view{argv[1]} == "--help") {
    std::cout << usage;
  } else {
    std::cout << "cxxp " << project_version << '\n';
  }
  return 0;
}
} // namespace cxxp

int main(int argc, char *argv[]) { return cxxp::run(argc, argv); }
