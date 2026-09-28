#ifndef CXXP_JSON_FILE_HXX
#define CXXP_JSON_FILE_HXX

#include <filesystem>
#include <nlohmann/json.hpp>

namespace cxxp {

nlohmann::json read_json(std::filesystem::path const &path);

} // namespace cxxp

#endif
