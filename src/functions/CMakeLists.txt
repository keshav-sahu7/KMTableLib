
set(KMT_FUNCTION_SOURCES
    ArithmeticFunctions.cpp
    ComparatorFunctions.cpp
    DateFunctions.cpp
    LogicalFunctions.cpp
    StringFunctions.cpp
    TypeFunctions.cpp
)

set(KMT_FUNCTION_LIB_NAME function)
add_library(${KMT_FUNCTION_LIB_NAME} STATIC ${KMT_FUNCTION_SOURCES})
add_library(KMTableLib::function ALIAS ${KMT_FUNCTION_LIB_NAME})

target_include_directories(${KMT_FUNCTION_LIB_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include/kmt)
