find_package(nlohmann_json REQUIRED)
find_package(valijson REQUIRED)

add_library(cxxp_manifest)
target_sources(cxxp_manifest PRIVATE
  cxxp/manifest.cxx
  cxxp/schema.cxx
)
target_sources(cxxp_manifest PUBLIC
  FILE_SET HEADERS
  BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}"
  FILES
      cxxp/manifest.hxx
      cxxp/schema.hxx
)
target_link_libraries(cxxp_manifest PUBLIC
  nlohmann_json::nlohmann_json
  valijson
)

configure_file(cxxp/version.cxx.in generated/cxxp/version.cxx @ONLY)
add_executable(cxxp)
target_sources(cxxp PRIVATE
  cxxp/main.cxx
  "${CMAKE_CURRENT_BINARY_DIR}/generated/cxxp/version.cxx"
)
target_sources(cxxp PRIVATE
  FILE_SET HEADERS
  BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}"
  FILES cxxp/version.hxx
)
target_link_libraries(cxxp PRIVATE
  cxxp_manifest
)
install(
  TARGETS cxxp
  COMPONENT cxxp
)
