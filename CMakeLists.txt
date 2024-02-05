
cmake_minimum_required(VERSION 3.21)

project(Thread_Pool)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Set compilation parameters
add_compile_options(-Wall -g -O0)
# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(./)

add_subdirectory(third_party)
add_subdirectory(core)
add_subdirectory(testcase)
add_subdirectory(examples)
add_subdirectory(benchmark)