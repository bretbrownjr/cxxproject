#include <csignal>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

namespace cxxp {

int helper(int argc, char **argv) {
  std::string mode = argc > 1 ? argv[1] : "";
  if (mode == "exit")
    return 37;
  if (mode == "signal") {
    std::raise(SIGTERM);
    return 1;
  }
  if (mode == "large") {
    std::cout << std::string(200000, 'x');
    return 0;
  }
  if (mode == "inspect") {
    std::cout << std::filesystem::current_path().string() << '\n';
    std::cout << (std::cin.get() == EOF ? "eof\n" : "input\n");
    for (int i = 2; i < argc; ++i)
      std::cout << std::string(argv[i]).size() << ':' << argv[i] << '\n';
    std::cerr << "helper diagnostic\n";
    return 0;
  }
  if (mode == "--style=file") {
    std::filesystem::path file = argv[argc - 1];
    std::ofstream("invocations", std::ios::app)
        << file.filename().string() << '\n';
    std::cout << "must be discarded\n";
    return file.filename() == "b-fail" ? 9 : 0;
  }
  return 2;
}

} // namespace cxxp

int main(int argc, char **argv) { return cxxp::helper(argc, argv); }
