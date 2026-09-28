foreach(location IN ITEMS repository outside-project)
  if(location STREQUAL "repository")
    set(working_directory "${PROJECT_SOURCE_DIR}")
  else()
    set(working_directory "")
  endif()
  set(cases help info-help no-arguments version unknown extra)
  if(location STREQUAL "repository")
    list(APPEND cases info info-nested)
  else()
    list(APPEND cases info-missing)
  endif()
  foreach(case IN LISTS cases)
    set(case_working_directory "${working_directory}")
    if(case STREQUAL "info-nested")
      set(case info)
      set(case_working_directory "${PROJECT_SOURCE_DIR}/src/cxxp")
      set(test_name "cli-${location}-info-nested")
    else()
      set(test_name "cli-${location}-${case}")
    endif()
    add_test(NAME "${test_name}"
      COMMAND "${CMAKE_COMMAND}"
        "-DEXECUTABLE=$<TARGET_FILE:cxxp>"
        "-DWORKING_DIRECTORY=${case_working_directory}"
        "-DCASE=${case}"
        "-DPROJECT_ROOT=${PROJECT_SOURCE_DIR}"
        "-DVERSION=${PROJECT_VERSION}"
        -P "${CMAKE_CURRENT_SOURCE_DIR}/cli.cmake")
    # Keep test-created temporary directories inside the CMake build tree.
    set_tests_properties("${test_name}" PROPERTIES
      ENVIRONMENT "TMPDIR=${CMAKE_CURRENT_BINARY_DIR};TEMP=${CMAKE_CURRENT_BINARY_DIR};TMP=${CMAKE_CURRENT_BINARY_DIR}")
  endforeach()
endforeach()

find_package(Catch2 REQUIRED)

add_executable(project-file-tests project_file.cxx)
target_link_libraries(project-file-tests PRIVATE
  cxxp_project_file
  Catch2::Catch2WithMain
)
target_compile_definitions(project-file-tests PRIVATE
  CXXP_TEST_FIXTURES="${CMAKE_CURRENT_SOURCE_DIR}/fixtures"
  CXXP_TEST_REPOSITORY_PROJECT_FILE="${PROJECT_SOURCE_DIR}/cxx.json"
  CXXP_TEST_PROJECT_VERSION="${PROJECT_VERSION}"
)
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/outside-project")

include(Catch)
# Keep Catch2 test-created temporary directories inside the CMake build tree.
catch_discover_tests(project-file-tests
  WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/outside-project"
  PROPERTIES ENVIRONMENT
    "TMPDIR=${CMAKE_CURRENT_BINARY_DIR};TEMP=${CMAKE_CURRENT_BINARY_DIR};TMP=${CMAKE_CURRENT_BINARY_DIR}"
)

add_library(cxxp_test_support)
target_sources(cxxp_test_support
  PRIVATE cxxp/test/test_support.cxx
  PUBLIC
    FILE_SET HEADERS
    BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}"
    FILES cxxp/test/test_support.hxx
)

add_executable(project-tests project.cxx)
target_link_libraries(project-tests PRIVATE
  cxxp_project_file
  cxxp_test_support
  Catch2::Catch2WithMain
)
# Keep Catch2 test-created temporary directories inside the CMake build tree.
catch_discover_tests(project-tests
  WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/outside-project"
  PROPERTIES ENVIRONMENT
    "TMPDIR=${CMAKE_CURRENT_BINARY_DIR};TEMP=${CMAKE_CURRENT_BINARY_DIR};TMP=${CMAKE_CURRENT_BINARY_DIR}"
)

add_test(NAME no-include-availability-probes
  COMMAND "${CMAKE_COMMAND}"
    "-DSOURCE_DIR=${PROJECT_SOURCE_DIR}"
    -P "${CMAKE_CURRENT_SOURCE_DIR}/no_include_probes.cmake")

add_executable(formatting-tests formatting.cxx)
target_link_libraries(formatting-tests PRIVATE
  cxxp_project_file cxxp_test_support Catch2::Catch2WithMain)
# Keep Catch2 test-created temporary directories inside the CMake build tree.
catch_discover_tests(formatting-tests
  WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/outside-project"
  PROPERTIES ENVIRONMENT
    "TMPDIR=${CMAKE_CURRENT_BINARY_DIR};TEMP=${CMAKE_CURRENT_BINARY_DIR};TMP=${CMAKE_CURRENT_BINARY_DIR}")

add_executable(process-helper process_helper.cxx)
add_executable(process-tests process.cxx)
target_link_libraries(process-tests PRIVATE
  cxxp_project_file cxxp_test_support Catch2::Catch2WithMain)
target_compile_definitions(process-tests PRIVATE
  CXXP_PROCESS_HELPER="$<TARGET_FILE:process-helper>")
add_dependencies(process-tests process-helper)
catch_discover_tests(process-tests
  WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/outside-project"
  PROPERTIES ENVIRONMENT
    "TMPDIR=${CMAKE_CURRENT_BINARY_DIR};TEMP=${CMAKE_CURRENT_BINARY_DIR};TMP=${CMAKE_CURRENT_BINARY_DIR}")
