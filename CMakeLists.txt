cmake_minimum_required(VERSION 3.21)

project(
  orm
  LANGUAGES CXX
)

set(exname orm)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(fmt REQUIRED CONFIG)

add_executable(${exname} src/main.cpp)

target_link_libraries(${exname} PRIVATE fmt::fmt)
