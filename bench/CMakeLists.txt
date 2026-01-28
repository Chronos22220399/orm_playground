# bench/CMakeLists.txt
cmake_minimum_required(VERSION 3.21)

# ============ Google Benchmark ============
include(FetchContent)

FetchContent_Declare(
    googlebenchmark
    GIT_REPOSITORY https://github.com/google/benchmark.git
    GIT_TAG        v1.8.3
)

set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
set(BENCHMARK_ENABLE_GTEST_TESTS OFF CACHE BOOL "" FORCE)
set(BENCHMARK_INSTALL_DOCS OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googlebenchmark)

# ============ ORM 源文件（与主项目共享） ============
file(GLOB ORM_SOURCES "${CMAKE_SOURCE_DIR}/src/ess/orm/*.cpp")

# ============ Benchmark 可执行文件 ============
add_executable(ess_orm_bench
    bench_main.cpp
    bench_insert.cpp
    bench_select.cpp
    bench_transaction.cpp
    ${ORM_SOURCES}  # 包含 ORM 源文件
)

target_include_directories(ess_orm_bench PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}              # bench/
    ${CMAKE_SOURCE_DIR}/include              # include/
    ${CMAKE_SOURCE_DIR}/configs              # configs/
)

# 链接库（与主项目相同）
target_link_libraries(ess_orm_bench PRIVATE
    benchmark::benchmark
    benchmark::benchmark_main
    fmt::fmt
    SQLite3
)

# Release 编译选项（不带 sanitizer）
target_compile_options(ess_orm_bench PRIVATE
    -O3
    -DNDEBUG
    -std=c++20
)
