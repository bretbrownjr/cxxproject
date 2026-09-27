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
#embed "../../schemas/manifest-v1.schema.json"
};

#if defined(__clang__)
#pragma clang diagnostic pop
#elif defined(__GNUC__)
#pragma GCC diagnostic pop
#endif
} // namespace

std::string_view const manifest_schema{
    reinterpret_cast<char const *>(schema_data), sizeof(schema_data)};
} // namespace cxxp
