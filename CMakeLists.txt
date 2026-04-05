cmake_minimum_required(VERSION 3.21)

project(
  orm
  LANGUAGES C CXX
  VERSION 0.0.1
)

set(exname orm)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


message(STATUS "=== C++ Compiler Info ===")
message(STATUS "CXX Compiler Path: ${CMAKE_CXX_COMPILER}")
message(STATUS "CXX Compiler ID: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "CXX Compiler Version: ${CMAKE_CXX_COMPILER_VERSION}")

function(add_external_subproject lib_folder target_name)
  set(lib_full_path "${CMAKE_CURRENT_SOURCE_DIR}/${lib_folder}")
  if(EXISTS "${lib_full_path}/CMakeLists.txt")
    message(STATUS "[ORM] Configuring external library: ${target_name}")

    # 特别配置 fmt
    if ("${target_name}" MATCHES "fmt")
      set(FMT_INSTALL OFF CACHE BOOL "" FORCE)
      set(FMT_TEST OFF CACHE BOOL "" FORCE)
    endif()
    
    add_subdirectory(${lib_full_path})

  else()
    message(FATAL_ERROR "[ORM] Library not found in ${lib_full_path}")
  endif()
endfunction()

add_external_subproject("thirdparties/fmt" "fmt")
add_external_subproject("thirdparties/sqlite3" "SQLite3")
add_external_subproject("thirdparties/expected" "expected")

file(GLOB_RECURSE ORM_SOURCES "src/ess/orm/*.cpp")

# common
add_library(
  ess_orm 
  src/main.cpp
  ${ORM_SOURCES}
)

target_include_directories(
  ess_orm
  PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/configs
  ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(
  ess_orm
  PRIVATE
  fmt::fmt
  SQLite3
  tl::expected
)

target_compile_options(
  ess_orm
  PRIVATE
  -O3
  -g 
  -fno-omit-frame-pointer
  -fsanitize=address
  -fsanitize=undefined
  # -fexceptions
  # -save-temps=obj
  # -Rpass=inline
)

target_compile_definitions(
  ess_orm
  PRIVATE
  ESS_ORM_EXPORTS
)

target_link_options(
  ess_orm
  PRIVATE
  -fsanitize=address
  -fsanitize=undefined
)


option(BUILD_BENCHMARKS "Build benchmarks" OFF)
#
# if(BUILD_BENCHMARKS)
#     add_subdirectory(bench)
# endif()

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
