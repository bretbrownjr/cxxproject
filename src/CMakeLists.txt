find_package(nlohmann_json REQUIRED)
find_package(valijson REQUIRED)

add_library(cxxp_impl)
target_sources(cxxp_impl PRIVATE
  cxxp/project_file.cxx
  cxxp/project.cxx
  cxxp/project_info.cxx
  cxxp/schema.cxx
  cxxp/json_file.cxx
  cxxp/lock_file.cxx
  cxxp/format_backend.cxx
  cxxp/format_project.cxx
  cxxp/format_runner.cxx
)
target_sources(cxxp_impl PUBLIC
  FILE_SET HEADERS
  BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}"
  FILES
      cxxp/project_file.hxx
      cxxp/project.hxx
      cxxp/project_info.hxx
      cxxp/schema.hxx
      cxxp/json_file.hxx
      cxxp/lock_file.hxx
      cxxp/format_backend.hxx
      cxxp/format_project.hxx
      cxxp/format_runner.hxx
      cxxp/process.hxx
)
target_link_libraries(cxxp_impl PUBLIC
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
  cxxp_impl
)
install(
  TARGETS cxxp
  COMPONENT cxxp
)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  target_sources(cxxp_impl PRIVATE cxxp/process_linux.cxx)
else()
  message(FATAL_ERROR "The process implementation currently requires Linux")
endif()
