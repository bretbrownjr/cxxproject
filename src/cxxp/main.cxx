#include <cxxp/version.hxx>

#include <iostream>
#include <string_view>

namespace cxxp {
namespace {
constexpr std::string_view usage = "Usage: cxxp [--help | --version]\n"
                                   "\n"
                                   "Options:\n"
                                   "  --help     Show this help\n"
                                   "  --version  Show the project version\n";
}

int run(int argc, char *argv[]) {
  if (argc == 1) {
    std::cout << usage;
    return 0;
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
