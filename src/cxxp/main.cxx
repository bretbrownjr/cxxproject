#include <cxxp/project.hxx>
#include <cxxp/project_info.hxx>
#include <cxxp/version.hxx>

#include <exception>
#include <iostream>
#include <string_view>

namespace cxxp {
namespace {
constexpr std::string_view usage = "Usage: cxxp [--help | --version]\n"
                                   "       cxxp info [--help]\n"
                                   "\n"
                                   "Options:\n"
                                   "  --help     Show this help\n"
                                   "  --version  Show the project version\n";
constexpr std::string_view info_usage =
    "Usage: cxxp info [--help]\n\n"
    "Show resolved project information as JSON.\n";
} // unnamed namespace

int run(int argc, char *argv[]) {
  if (argc == 1) {
    std::cout << usage;
    return 0;
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
