#include <cxxp/schema.hxx>

namespace cxxp {

namespace {

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc23-extensions"
#elif defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wc++26-extensions"
#endif

constexpr unsigned char schema_data[] = {
#embed "../../schemas/cxx-json-v1.schema.json"
};

constexpr unsigned char lock_schema_data[] = {
#embed "../../schemas/cxx-lock-json-v1.schema.json"
};

#if defined(__clang__)
#pragma clang diagnostic pop
#elif defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

} // unnamed namespace

std::string_view const project_file_schema{
    reinterpret_cast<char const *>(schema_data), sizeof(schema_data)};

std::string_view const lock_file_schema{
    reinterpret_cast<char const *>(lock_schema_data), sizeof(lock_schema_data)};

} // namespace cxxp
