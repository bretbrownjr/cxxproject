#ifndef CXXP_FORMAT_BACKEND_HXX
#define CXXP_FORMAT_BACKEND_HXX

#include <filesystem>
#include <string>
#include <string_view>
#include <vector>

namespace cxxp {

enum class FormattingBackend { clang_format };
enum class FormatOperation { rewrite, check };

// Release identity and local installation are deliberately separate.
struct ToolIdentity {
  std::string version;
  std::string diagnostic_output;
};

class ClangFormatBackend {
public:
  static constexpr std::string_view tool_id = "clang-format";

  static bool valid_release(std::string_view version);
  static bool supports(std::string_view version);

  static ToolIdentity identify(std::string const &output);
  static std::vector<std::string> identification_arguments();

  // The frontend must validate the selected release and lock before execution.
  static std::vector<std::string> arguments(FormatOperation operation,
                                            std::filesystem::path const &file);
};

} // namespace cxxp

#endif
