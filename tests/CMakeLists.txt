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