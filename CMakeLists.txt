cmake_minimum_required(VERSION 4.4)
project(cxxproject
  LANGUAGES CXX
  VERSION 0.1.5
)
include(CTest)

add_subdirectory(src)
if(BUILD_TESTING)
  add_subdirectory(tests)
endif()
