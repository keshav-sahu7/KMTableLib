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

enable_testing()

# build the unit test
add_subdirectory(tests)

# build example
add_subdirectory(Example)


install(DIRECTORY include/kmt DESTINATION include)