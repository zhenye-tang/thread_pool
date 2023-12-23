
cmake_minimum_required(VERSION 3.21)

project(thread_pool_test)

file(GLOB SOURCES *.cpp)

# Set compilation parameters
add_compile_options(-Wall -g -O0)
# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Generate executable
add_executable(
        thread_pool_test
        ${SOURCES}
)

target_link_libraries(thread_pool_test gtest_main)
include(GoogleTest)
enable_testing()
gtest_discover_tests(thread_pool_test)
