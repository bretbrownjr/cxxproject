#include <cxxp/project_info.hxx>

namespace cxxp {
void to_json(nlohmann::ordered_json &output, Project const &project) {
  using Json = nlohmann::ordered_json;
  auto &result = output;
  result = Json::object();
  result["outputVersion"] = 1;
  result["project"] = {{"name", project.name}, {"version", project.version}};
  result["root"] = project.root.generic_string();
  result["projectFile"] = "cxx.json";
  result["files"] = {{"sources", project.sources},
                     {"headers", project.headers}};
  result["tools"] = {
      {"clang-format",
       {{"configurationFiles", project.clang_format_configurations}}},
      {"clang-tidy",
       {{"configurationFiles", project.clang_tidy_configurations}}}};
  result["build"] = Json::object();
  switch (project.backend) {
  case BuildBackend::none:
    result["build"]["backend"] = nullptr;
    result["build"]["entryPoint"] = nullptr;
    break;
  case BuildBackend::cmake:
    result["build"]["backend"] = "cmake";
    result["build"]["entryPoint"] = "CMakeLists.txt";
    break;
  }
}
} // namespace cxxp
