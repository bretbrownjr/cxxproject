configure_file(version.cpp.in generated/version.cpp @ONLY)
add_executable(cxxp main.cxx "${CMAKE_CURRENT_BINARY_DIR}/generated/version.cpp")
target_sources(cxxp PRIVATE
  FILE_SET HEADERS
  BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}"
  FILES cxxp/version.hxx
)
target_link_libraries(cxxp PRIVATE nlohmann_json::nlohmann_json valijson)
