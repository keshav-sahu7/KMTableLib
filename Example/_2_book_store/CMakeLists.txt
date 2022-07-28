cmake_minimum_required(VERSION 3.5)


#include kmtable headers
include_directories(${CMAKE_SOURCE_DIR}/include)

set(TARGET_2 BookStore)
#create test executable
add_executable(${TARGET_2} main.cpp)

set_target_properties( ${TARGET_2}
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

include_directories(../../include)

target_link_directories(${TARGET_2} PUBLIC ../../build/lib)

target_link_libraries(${TARGET_2} KMTableLib)
