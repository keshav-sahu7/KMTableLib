cmake_minimum_required(VERSION 3.17)

project(KMTableLib VERSION 0.2 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(NOT MSVC)
    add_compile_options(-Wall -Wextra -Wpedantic -fPIC)
endif()


set(CMAKE_DEBUG_POSTFIX "-d")
set(K_BUILD_DIR ${CMAKE_BUILD_TYPE})

# main target resides in /src
add_subdirectory(src)

#########################################################################################
################################GooogleTest[Settings]####################################

set(GoogleTestFrameWorkAvailable TRUE)

if ($ENV{GOOGLETEST_DIR})
    set(GOOGLETEST_DIR $ENV{GOOGLETEST_DIR})
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
    set(GoogleTestFrameworkAvailable FALSE)
    message( WARNING "No googletest src dir found - set GOOGLETEST_DIR to enable!")
endif ()


if (GoogleTestFrameworkAvailable)
    enable_testing()
    # build the unit test
    add_subdirectory(tests)
else()
    message( WARNING "Proceeding without google unit test.")
endif()

# build example
add_subdirectory(Example)


install(DIRECTORY include/kmt DESTINATION include)
