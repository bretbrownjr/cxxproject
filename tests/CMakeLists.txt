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
