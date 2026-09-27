foreach(location IN ITEMS repository outside-project)
  if(location STREQUAL "repository")
    set(working_directory "${PROJECT_SOURCE_DIR}")
  else()
    set(working_directory "")
  endif()
  foreach(case IN ITEMS help no-arguments version unknown extra)
    add_test(NAME "cli-${location}-${case}"
      COMMAND "${CMAKE_COMMAND}"
        "-DEXECUTABLE=$<TARGET_FILE:cxxp>"
        "-DWORKING_DIRECTORY=${working_directory}"
        "-DCASE=${case}"
        "-DVERSION=${PROJECT_VERSION}"
        -P "${CMAKE_CURRENT_SOURCE_DIR}/cli.cmake")
  endforeach()
endforeach()

find_package(Catch2 REQUIRED)

add_executable(manifest-tests manifest.cxx)
target_link_libraries(manifest-tests PRIVATE
  cxxp_manifest
  Catch2::Catch2WithMain
)
target_compile_definitions(manifest-tests PRIVATE
  CXXP_TEST_FIXTURES="${CMAKE_CURRENT_SOURCE_DIR}/fixtures"
  CXXP_TEST_REPOSITORY_MANIFEST="${PROJECT_SOURCE_DIR}/cxx.json"
  CXXP_TEST_PROJECT_VERSION="${PROJECT_VERSION}"
)
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/outside-project")

include(Catch)
catch_discover_tests(manifest-tests
  WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/outside-project"
)
