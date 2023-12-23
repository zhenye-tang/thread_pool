cmake_minimum_required(VERSION 3.21)

project(therad_pool_example)

file(GLOB SOURCES *.c)

# Generate executable
add_executable(therad_pool_example
        ${SOURCES}
)

target_link_libraries(therad_pool_example thread-pool)