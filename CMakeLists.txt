
cmake_minimum_required(VERSION 3.21)

project(thread_pool)

# Set compilation parameters
add_compile_options(-Wall -g -O0)
# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(./)

# Generate executable
add_executable(${CMAKE_PROJECT_NAME} 
            ./main.c
            ./thread_pool.c
            )

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(${CMAKE_PROJECT_NAME} Threads::Threads)