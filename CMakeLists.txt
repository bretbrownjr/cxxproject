cmake_minimum_required(VERSION 4.4)
project(cxxproject
  LANGUAGES CXX
  VERSION 0.2
)
include(CTest)
find_package(nlohmann_json 3.11 CONFIG REQUIRED)
find_package(valijson CONFIG REQUIRED)

add_subdirectory(src)
if(BUILD_TESTING)
  add_subdirectory(tests)
endif()
