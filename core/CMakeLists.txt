file(GLOB SOURCES *.c)
add_library(thread-pool ${SOURCES})
target_include_directories(thread-pool PUBLIC .)