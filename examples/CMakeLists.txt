# Examples for ess_orm library

# Set C++20 standard for examples
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Example 1: Basic ORM usage
add_executable(orm_example main.cpp)
target_link_libraries(orm_example ess_orm)

# Include directories for examples
target_include_directories(orm_example PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include)
target_include_directories(orm_example PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../configs)

# Example 2: Schema demonstration
add_executable(schema_example schema.cpp)
target_link_libraries(schema_example ess_orm)
target_include_directories(schema_example PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include)
target_include_directories(schema_example PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../configs)

# Use same compile options as the library
target_compile_options(orm_example PRIVATE
  -O3
  -g
  -fno-omit-frame-pointer
  -fsanitize=address
  -fsanitize=undefined
)

target_compile_options(schema_example PRIVATE
  -O3
  -g
  -fno-omit-frame-pointer
  -fsanitize=address
  -fsanitize=undefined
)

target_link_options(orm_example PRIVATE
  -fsanitize=address
  -fsanitize=undefined
)

target_link_options(schema_example PRIVATE
  -fsanitize=address
  -fsanitize=undefined
)
