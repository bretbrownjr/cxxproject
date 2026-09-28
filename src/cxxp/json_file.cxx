#include <cxxp/json_file.hxx>
#include <fstream>
#include <map>
#include <set>
#include <stdexcept>

namespace cxxp {

using Json = nlohmann::json;

Json read_json(std::filesystem::path const &path) {
  std::ifstream input(path);
  if (!input) {
    throw std::runtime_error("cannot read '" + path.string() + "'");
  }
  std::map<int, std::set<std::string>> keys;
  auto const callback = [&keys, &path](int depth, Json::parse_event_t event,
                                       Json &parsed) {
    if (event == Json::parse_event_t::object_start) {
      keys[depth + 1].clear();
    } else if (event == Json::parse_event_t::key &&
               !keys[depth].insert(parsed.get<std::string>()).second) {
      throw std::runtime_error("'" + path.string() +
                               "': duplicate JSON object key '" +
                               parsed.get<std::string>() + "'");
    }
    return true;
  };
  try {
    return Json::parse(input, callback);
  } catch (Json::parse_error const &error) {
    throw std::runtime_error("'" + path.string() +
                             "': malformed JSON: " + error.what());
  }
}

} // namespace cxxp
