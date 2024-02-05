include(GoogleTest)

function(add_test_framework TARGET)
    file(GLOB SOURCES *.cpp **/*.cpp)
    add_executable(${TARGET} ${SOURCES})
    gtest_discover_tests(${TARGET})
    target_link_libraries(${TARGET} gtest_main)
    target_link_libraries(${TARGET} benchmark)
endfunction()

macro(get_subdir result curdir)
    file(GLOB subitem REALTIVE ${curdir}/*)
    set(dirlist "")
    foreach(item ${subitem})
        if(IS_DIRECTORY ${item})
            list(APPEND dirlist ${item})
        endif()
    endforeach()
    set(${result} ${dirlist})
 endmacro()

 get_subdir(SUBDIRS ${CMAKE_CURRENT_LIST_DIR})

 foreach(subdir ${SUBDIRS})
    add_subdirectory(${subdir})
 endforeach()
 