cmake_minimum_required(VERSION 3.5)

find_package(Threads REQUIRED)

#########################################################################################
################################GooogleTest[Settings]####################################
if ($ENV{GOOGLETEST_DIR})
    set(GOOGLETEST_DIR $ENV{GOOGLETEST_DIR})
else ()
    if (NOT "C:/Users/Windows10/googletest" STREQUAL "")
        message(WARNING "Using googletest src dir specified at Qt Creator wizard")
    endif ()
    set(GOOGLETEST_DIR "C:/Users/Windows10/googletest")
endif ()
if (EXISTS ${GOOGLETEST_DIR})
    set(GTestSrc ${GOOGLETEST_DIR}/googletest)
    set(GMockSrc ${GOOGLETEST_DIR}/googlemock)
elseif (UNIX AND EXISTS /usr/src/gtest)
    set(GTestSrc /usr/src/gtest)
    message(WARNING "Using gtest from system")
    if (EXISTS /usr/src/gmock)
        set(GMockSrc /usr/src/gmock)
    endif ()
else ()
    message( FATAL_ERROR "No googletest src dir found - set GOOGLETEST_DIR to enable!")
endif ()

set(GTestFiles ${GTestSrc}/src/gtest-all.cc)
set(GTestIncludes ${GTestSrc} ${GTestSrc}/include)
if (NOT ${GMockSrc} STREQUAL "")
    list(APPEND GTestFiles ${GMockSrc}/src/gmock-all.cc)
    list(APPEND GTestIncludes ${GMockSrc} ${GMockSrc}/include)
endif ()


include_directories(${GTestIncludes})
###############################GoogleTest[Settings]#######################################

#include kmtable headers
include_directories(${CMAKE_SOURCE_DIR}/include)

#create test executable
add_executable(test_table
        main.cpp
        test_helper.cpp
        test_helper.hpp
        tst_basicview.cpp
        tst_core.cpp
        tst_csvwriter.cpp
        tst_table.cpp
        tst_tableio.cpp
        ${GTestFiles}
)

#add test
add_test(NAME table COMMAND ${CMAKE_BINARY_DIR}/bin/test_table)

set_target_properties( test_table
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${K_BUILD_DIR}/bin"
)

target_link_directories(test_table PUBLIC ${CMAKE_BINARY_DIR}/${K_BUILD_DIR}/lib)
target_link_libraries(test_table PUBLIC KMTableLib)
target_link_libraries(test_table PRIVATE Threads::Threads)

if(NOT MSVC)
	target_compile_options(test_table PUBLIC ${CMAKE_CXX_FLAGS} -Wno-sign-compare)
endif()