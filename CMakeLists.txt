cmake_minimum_required(VERSION 3.18)
project(PSO VERSION 0.1.5)

find_package(FMT)
find_package(range-v3 CONFIG REQUIRED)
SET(CMAKE_CXX_COMPILER "/usr/bin/clang++-10")
SET(CMAKE_EXPORT_COMPILE_COMMANDS "ON")

if(EXISTS ${CMAKE_CURRENT_BINARY_DIR}/conaninfo.txt)
  set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_BINARY_DIR})
endif()

find_package(toml11)
include_directories(
  include
  )

set(LIB_SOURCES
  "${CMAKE_CURRENT_SOURCE_DIR}/src/pso.cpp")
add_library(libpso
  STATIC
  ${LIB_SOURCES})
target_include_directories(libpso
  PUBLIC
  include)
target_compile_features(libpso
  PUBLIC
  cxx_std_11)
target_link_libraries(
  libpso
  PUBLIC
  toml11::toml11
  )
# Example
set(EXAMPLE_SOURCES
"${CMAKE_CURRENT_SOURCE_DIR}/examples/main.cpp")
add_executable(pso_example ${EXAMPLE_SOURCES})
target_link_libraries(pso_example
  PRIVATE
  fmt::fmt
  toml11::toml11
  libpso)
target_compile_features(pso_example
  PRIVATE
  cxx_std_11)
