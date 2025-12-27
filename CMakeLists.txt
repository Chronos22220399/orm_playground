cmake_minimum_required(VERSION 3.21)

project(
  orm
  LANGUAGES CXX
)

set(exname orm)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

message(STATUS "=== C++ Compiler Info ===")
message(STATUS "CXX Compiler Path: ${CMAKE_CXX_COMPILER}")
message(STATUS "CXX Compiler ID: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "CXX Compiler Version: ${CMAKE_CXX_COMPILER_VERSION}")

find_package(fmt REQUIRED CONFIG)

add_executable(${exname} src/main.cpp)
# add_executable(${exname} src/schema.cpp)


target_link_libraries(${exname} PRIVATE fmt::fmt)
