# Tests for ORM runtime functionality
cmake_minimum_required(VERSION 3.21)

# Find GoogleTest
find_package(GTest REQUIRED)

# Test executables
add_executable(test_runtime
    example_based_test.cpp
)

# Use test configuration
target_compile_definitions(test_runtime
    PRIVATE
    ESS_ORM_USER_CONFIG="configs/test_config.hpp"
)

target_link_libraries(test_runtime
    PRIVATE
    ess_orm
    GTest::gtest
    GTest::gtest_main
    ${SQLITE3_TARGET}
)

target_include_directories(test_runtime
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/configs
)

# Add test
add_test(NAME runtime_tests COMMAND test_runtime)

# Enable test discovery
include(GoogleTest)
gtest_discover_tests(test_runtime)

# Compile-time SQL parser edge case tests (static_assert based)
add_executable(sql_parser_edge_test
    sql_parser_edge_test.cpp
)
target_compile_features(sql_parser_edge_test PRIVATE cxx_std_20)
target_include_directories(sql_parser_edge_test
    PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/configs
)
add_test(NAME sql_parser_edge_test
    COMMAND sql_parser_edge_test
)