file(GLOB_RECURSE source_files LIST_DIRECTORIES false
  "${SOURCE_DIR}/src/*.cxx"
  "${SOURCE_DIR}/src/*.hxx"
  "${SOURCE_DIR}/tests/*.cxx"
  "${SOURCE_DIR}/tests/*.hxx")

foreach(source_file IN LISTS source_files)
  file(READ "${source_file}" source_contents)
  if(source_contents MATCHES "__has_include")
    message(FATAL_ERROR
      "Header availability probing is disallowed: ${source_file}")
  endif()
endforeach()
