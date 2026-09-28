# Set up a project with a formatter that reports its version but fails formatting.
set(root "${CMAKE_CURRENT_BINARY_DIR}/format-command-project")
file(REMOVE_RECURSE "${root}")
file(MAKE_DIRECTORY "${root}/bin")
file(WRITE "${root}/cxx.json" [[
{
  "schemaVersion": 1,
  "project": {
    "name": "test",
    "version": "1"
  }
}
]])
file(WRITE "${root}/bin/clang-format" [[#!/bin/sh
if [ "$1" = --version ]; then
  echo 'clang-format version 22.1.8'
  exit
fi
printf '%s\n' "$@" >> calls
exit 1
]])
file(CHMOD "${root}/bin/clang-format"
  PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
)
set(ENV{PATH} "${root}/bin")

# Commands must return the expected status without writing to stdout.
function(run expected)
  execute_process(
    COMMAND "${EXECUTABLE}" ${ARGN}
    WORKING_DIRECTORY "${root}"
    RESULT_VARIABLE status
    OUTPUT_VARIABLE output
    ERROR_VARIABLE error
  )
  if(NOT status STREQUAL "${expected}" OR NOT output STREQUAL "")
    message(FATAL_ERROR "${ARGN}: ${status}: ${output}; ${error}")
  endif()
endfunction()

# Locked mode requires existing metadata, even with no source files.
run(1 format --locked)
if(EXISTS "${root}/cxx.lock.json")
  message(FATAL_ERROR "--locked wrote metadata")
endif()

# An empty inventory creates a lock without invoking the formatter.
run(0 format)
if(EXISTS "${root}/calls" OR NOT EXISTS "${root}/cxx.lock.json")
  message(FATAL_ERROR "empty inventory was not locked without formatting")
endif()

# Check mode visits all files in sorted order despite formatter failures.
file(WRITE "${root}/a.hxx" "unchanged")
file(WRITE "${root}/z.cxx" "unchanged")
run(1 format --check --locked)
file(READ "${root}/calls" calls)
if(NOT calls MATCHES "--dry-run\n--Werror\n${root}/a.hxx.*${root}/z.cxx")
  message(FATAL_ERROR "check did not visit sorted headers and sources: ${calls}")
endif()

# Rewrite mode stops after the first failure.
file(REMOVE "${root}/calls")
run(1 format)
file(READ "${root}/calls" calls)
if(calls MATCHES "z.cxx" OR NOT calls MATCHES "-i\n${root}/a.hxx")
  message(FATAL_ERROR "rewrite did not stop at first failure: ${calls}")
endif()

# A version mismatch preserves the lock and prevents formatting.
file(REMOVE "${root}/calls")
file(READ "${root}/cxx.lock.json" original)
string(REPLACE "22.1.8" "21.1.0" mismatch "${original}")
file(WRITE "${root}/cxx.lock.json" "${mismatch}")
run(1 format)
run(1 lock)
file(READ "${root}/cxx.lock.json" preserved)
if(NOT preserved STREQUAL mismatch OR EXISTS "${root}/calls")
  message(FATAL_ERROR "mismatch changed lock or invoked formatting")
endif()

# An explicit tool update repairs the lock without formatting sources.
run(0 lock --update-tool clang-format)
file(READ "${root}/cxx.lock.json" updated)
if(NOT updated STREQUAL original OR EXISTS "${root}/calls")
  message(FATAL_ERROR "explicit update failed or formatted sources")
endif()

# Reject duplicate flags, incompatible flags, and unsupported tool names.
foreach(arguments IN ITEMS
  "format;--check;--check"
  "format;--help;--locked"
  "lock;--locked"
  "lock;--update-tool;other"
)
  run(1 ${arguments})
endforeach()

file(REMOVE_RECURSE "${root}")
