#ifndef CXXP_LOCK_FILE_HXX
#define CXXP_LOCK_FILE_HXX

#include <filesystem>
#include <nlohmann/json.hpp>
#include <optional>
#include <span>
#include <string>
#include <string_view>

namespace cxxp {

// Opaque dependency and unrelated tool records survive updates unchanged.
class LockFile {
public:
  LockFile();
  explicit LockFile(nlohmann::json document);

  static std::optional<LockFile> load(std::filesystem::path const &path);

  nlohmann::json const &document() const;

  std::optional<std::string> version(std::string_view tool) const;

  void require_tools(std::span<std::string_view const> tools) const;

  void set_tool(std::string_view tool, std::string_view version);

  // Linux: exclusive temporary file in the destination directory, then rename.
  void write_atomic(std::filesystem::path const &path) const;

private:
  nlohmann::json document_;
};

} // namespace cxxp

#endif
