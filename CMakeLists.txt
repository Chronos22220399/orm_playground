cmake_minimum_required(VERSION 3.21)

project(
  orm
  LANGUAGES CXX
)


set(exname orm)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# include(FetchContent)
# FetchContent_Declare(
#   doctest
#   GIT_REPOSITORY https://github.com/doctest/doctest.git
#   GIT_TAG v2.4.11
# )
# FetchContent_MakeAvailable(doctest)


message(STATUS "=== C++ Compiler Info ===")
message(STATUS "CXX Compiler Path: ${CMAKE_CXX_COMPILER}")
message(STATUS "CXX Compiler ID: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "CXX Compiler Version: ${CMAKE_CXX_COMPILER_VERSION}")

find_package(fmt REQUIRED CONFIG)
find_package(SQLite3 REQUIRED)

# common
add_executable(
  ${exname} 
  # src/leetcode.cpp
  # src/main.cpp
  src/schema.cpp
  # src/test_sqlite3.cpp
)

target_include_directories(
  ${exname}
  PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/configs
)

target_link_libraries(
  ${exname} 
  PRIVATE
  fmt::fmt
  ${SQLite3_LIBRARIES}
)

set(LLVM_PREFIX "/opt/homebrew/opt/llvm")

target_compile_options(
  ${exname}
  PRIVATE
  -O2
  -fsanitize=address
  -stdlib=libc++
  # -save-temps=obj
  # -Rpass=inline
)

target_link_options(
  ${exname}
  PRIVATE
  -fsanitize=address
  -stdlib=libc++
  -L${LLVM_PREFIX}/lib/c++
  -W1,-rpath,${LLVM_PREFIX}/lib/c++
)

# target_precompile_headers(
#   ${exname}
#   PRIVATE
#   include/std.hpp
#   include/core_thirdparties.hpp
#   include/core.hpp
# )

# tests
# enable_testing()
# add_executable(
#   orm_tests
#   tests/test_main.cpp
#   tests/test_schema.cpp
# )
#
# target_include_directories(
#   orm_tests
#   PRIVATE
#   ${CMAKE_CURRENT_SOURCE_DIR}/include
# )
#
# target_link_libraries(
#   orm_tests
#   PRIVATE
#   doctest::doctest
#   fmt::fmt
# )
#
# target_compile_options(
#   orm_tests
#   PRIVATE
#   -O0
#   -g
#   -fsanitize=address
#   # -save-temps=obj
#   # -Rpass=inline
# )
#
# target_link_options(
#   orm_tests
#   PRIVATE
#   -fsanitize=address
# )
#
# target_precompile_headers(
#   orm_tests
#   PRIVATE
#   include/std.hpp
#   include/core_thirdparties.hpp
#   include/core.hpp
# )
#
# add_test(NAME orm_tests COMMAND orm_tests)
