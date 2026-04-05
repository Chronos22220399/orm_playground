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

# 固定使用动态库 + 系统 SQLite3
set(BUILD_SHARED_LIBS ON)
set(USE_SYSTEM_SQLITE3 ON)

message(STATUS "=== C++ Compiler Info ===")
message(STATUS "CXX Compiler Path: ${CMAKE_CXX_COMPILER}")
message(STATUS "CXX Compiler ID: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "CXX Compiler Version: ${CMAKE_CXX_COMPILER_VERSION}")

# 查找系统 SQLite3
find_package(SQLite3 REQUIRED)
if(NOT TARGET SQLite3::SQLite3)
    add_library(SQLite3::SQLite3 INTERFACE IMPORTED)
    set_target_properties(SQLite3::SQLite3 PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${SQLite3_INCLUDE_DIRS}"
        INTERFACE_LINK_LIBRARIES "${SQLite3_LIBRARIES}"
    )
endif()
set(SQLITE3_TARGET SQLite3::SQLite3)
message(STATUS "Using system SQLite3: ${SQLite3_VERSION}")

# 构建库
add_subdirectory(src)

# Examples
add_subdirectory(examples)

option(BUILD_BENCHMARKS "Build benchmarks" OFF)
