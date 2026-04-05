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

# 构建类型选项
option(BUILD_SHARED_LIBS "Build ess_orm as shared library (dynamic)" ON)
option(USE_SYSTEM_SQLITE3 "Use system SQLite3 library (instead of bundled)" ON)
option(ENABLE_MYSQL "Enable MySQL database support (placeholder)" OFF)
option(ENABLE_POSTGRES "Enable PostgreSQL database support (placeholder)" OFF)


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

if(USE_SYSTEM_SQLITE3)
    find_package(SQLite3 REQUIRED)
    if(NOT TARGET SQLite3::SQLite3)
        # Create imported target if not provided by FindSQLite3
        add_library(SQLite3::SQLite3 INTERFACE IMPORTED)
        set_target_properties(SQLite3::SQLite3 PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${SQLite3_INCLUDE_DIRS}"
            INTERFACE_LINK_LIBRARIES "${SQLite3_LIBRARIES}"
        )
    endif()
    set(SQLITE3_TARGET SQLite3::SQLite3)
    message(STATUS "Using system SQLite3: ${SQLite3_VERSION}")
else()
    add_external_subproject("thirdparties/sqlite3" "SQLite3")
    set(SQLITE3_TARGET SQLite3)
    message(STATUS "Using bundled SQLite3")
endif()

# MySQL 占位支持
if(ENABLE_MYSQL)
    message(STATUS "MySQL support enabled (placeholder)")
    # 未来添加: find_package(MySQL REQUIRED)
    # 未来添加: set(MYSQL_TARGET ...)
endif()

# PostgreSQL 占位支持
if(ENABLE_POSTGRES)
    message(STATUS "PostgreSQL support enabled (placeholder)")
    # 未来添加: find_package(PostgreSQL REQUIRED)
    # 未来添加: set(POSTGRES_TARGET ...)
endif()

add_external_subproject("thirdparties/expected" "expected")

# 构建库
add_subdirectory(src)

# Examples


# Examples
add_subdirectory(examples)

option(BUILD_BENCHMARKS "Build benchmarks" OFF)
