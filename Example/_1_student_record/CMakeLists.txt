#cmake_minimum_required(VERSION 3.20)

#set(CMAKE_CXX_STANDARD 17)

#set(CMAKE_CXX_STANDARD_REQUIRED on)

#project(KMTableExamples VERSION 1.0 LANGUAGES CXX)

set(TARGET_1 StudentRecord)

add_executable(${TARGET_1} main.cpp StudentRecord.cpp StudentRecord.hpp Marks.hpp)

include_directories(../../include)

target_link_directories(${TARGET_1} PUBLIC ../../build/lib)

target_link_libraries(${TARGET_1} KMTableLib)

set_target_properties( ${TARGET_1}
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)