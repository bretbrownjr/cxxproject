#include <cxxp/test/test_support.hxx>

#include <fstream>
#include <random>
#include <stdexcept>
#include <string>
#include <system_error>

namespace cxxp::test {
TemporaryProject::TemporaryProject() {
  std::random_device random;
  auto const temporary_root = std::filesystem::temp_directory_path();
  for (;;) {
    root_ = temporary_root / ("cxxp-project-" + std::to_string(random()));
    std::error_code error;
    if (std::filesystem::create_directory(root_, error)) {
      return;
    }
    if (error) {
      throw std::runtime_error("cannot create temporary project '" +
                               root_.string() + "': " + error.message());
    }
  }
}

TemporaryProject::~TemporaryProject() {
  std::error_code error;
  std::filesystem::remove_all(root_, error);
}

std::filesystem::path const &TemporaryProject::root() const noexcept {
  return root_;
}

void TemporaryProject::write(NamedFile const &file) const {
  auto const path = root_ / file.filename;
  std::filesystem::create_directories(path.parent_path());
  std::ofstream output(path);
  if (!output) {
    throw std::runtime_error("cannot create test file '" + path.string() + "'");
  }
  output << file.contents;
  if (!output) {
    throw std::runtime_error("cannot write test file '" + path.string() + "'");
  }
}

void TemporaryProject::touch(std::filesystem::path const &relative) const {
  write(NamedFile{relative, {}});
}
} // namespace cxxp::test
