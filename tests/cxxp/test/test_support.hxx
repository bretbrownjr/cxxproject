#ifndef CXXP_TEST_TEST_SUPPORT_HXX
#define CXXP_TEST_TEST_SUPPORT_HXX

#include <filesystem>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

namespace cxxp::test {

/// Distinguishes a project's filename from its contents to prevent transposed
/// arguments.
class NamedFile {
public:
  NamedFile(std::filesystem::path filename, std::string_view contents)
      : filename(std::move(filename)), contents(contents) {}

  std::filesystem::path filename;
  std::string contents;
};

/// Provides an isolated temporary project directory for tests.
class TemporaryProject {
public:
  TemporaryProject();
  ~TemporaryProject();

  TemporaryProject(TemporaryProject const &) = delete;
  TemporaryProject &operator=(TemporaryProject const &) = delete;

  std::filesystem::path const &root() const noexcept;
  void write(NamedFile const &file) const;
  template <typename... Files>
    requires(sizeof...(Files) > 0 &&
             (std::is_same_v<std::remove_cvref_t<Files>, NamedFile> && ...))
  void write(NamedFile const &first, Files const &...files) const {
    write(first);
    (write(files), ...);
  }
  void touch(std::filesystem::path const &relative) const;
  template <typename... Paths>
    requires(sizeof...(Paths) > 1)
  void touch(Paths &&...paths) const {
    (touch(std::filesystem::path(std::forward<Paths>(paths))), ...);
  }

private:
  std::filesystem::path root_;
};

} // namespace cxxp::test

#endif // CXXP_TEST_TEST_SUPPORT_HXX
