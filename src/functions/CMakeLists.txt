
set(KMT_FUNCTION_SOURCES
    Arithmeticfunctions.cpp
    Comparatorfunctions.cpp
    DateFunctions.cpp
    Logicalfunctions.cpp
    Stringfunctions.cpp
    Typefunctions.cpp
)

set(KMT_FUNCTION_LIB_NAME function)
add_library(${KMT_FUNCTION_LIB_NAME} STATIC ${KMT_FUNCTION_SOURCES})
add_library(KMTableLib::function ALIAS ${KMT_FUNCTION_LIB_NAME})

target_include_directories(${KMT_FUNCTION_LIB_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include/kmt)
