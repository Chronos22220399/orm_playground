# Benchmark tests for ORM vs SQLite3 comparison
cmake_minimum_required(VERSION 3.21)

# Find Google Benchmark
find_package(benchmark REQUIRED)

# Benchmark executables
add_executable(comprehensive_benchmark
    comprehensive_benchmark.cpp
)

add_executable(file_benchmark
    file_benchmark.cpp
)

# Link libraries for all benchmarks
target_link_libraries(comprehensive_benchmark
    PRIVATE
    ess_orm
    benchmark::benchmark
    ${SQLITE3_TARGET}
)

target_link_libraries(file_benchmark
    PRIVATE
    ess_orm
    benchmark::benchmark
    ${SQLITE3_TARGET}
)

# Include directories for all benchmarks
target_include_directories(comprehensive_benchmark
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/configs
)

target_include_directories(file_benchmark
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/configs
)

# Add benchmark targets
add_custom_target(run_comprehensive_benchmark
    COMMAND comprehensive_benchmark
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS comprehensive_benchmark
    COMMENT "Running comprehensive SQLite3 vs ORM benchmark tests"
)

add_custom_target(run_file_benchmark
    COMMAND file_benchmark
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS file_benchmark
    COMMENT "Running file database SQLite3 vs ORM benchmark tests"
)