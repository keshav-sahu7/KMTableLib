
set(
    KMT_SOURCES
    
    AbstractTable.cpp
    AbstractView.cpp
    BasicView.cpp
    Core.cpp
    ErrorHandler.cpp
    FunctionStore.cpp
    LogMsg.cpp
    Printer.cpp
    TableIO.cpp
    CSVWriter.cpp
    Parser2.cpp
    Table.cpp
    Types.cpp

    KException.h
    LogFileHelper.h
)

set(
    KMT_HEADERS


    ../include/kmt/AbstractTable.hpp
    ../include/kmt/AbstractView.hpp
    ../include/kmt/BasicView.hpp
    ../include/kmt/Column.hpp
    ../include/kmt/Core.hpp
    ../include/kmt/CSVWriter.hpp
    ../include/kmt/ErrorHandler.hpp
    ../include/kmt/FunctionStore.hpp
    ../include/kmt/LogMsg.hpp
    ../include/kmt/Parser2.hpp
    ../include/kmt/Printer.hpp
    ../include/kmt/Table.hpp
    ../include/kmt/TableIO.hpp
    ../include/kmt/Types.hpp
    ../include/kmt/TypeTraits.hpp
)


add_subdirectory(functions)

add_library(${CMAKE_PROJECT_NAME} SHARED ${KMT_SOURCES} ${KMT_HEADERS})

#our main target KMTable.so or KMTable.dll
set_target_properties( ${CMAKE_PROJECT_NAME}
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${K_BUILD_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${K_BUILD_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${K_BUILD_DIR}/bin"
)

target_include_directories(${CMAKE_PROJECT_NAME}
    PUBLIC ${PROJECT_SOURCE_DIR}/include/kmt
)

target_link_directories(${CMAKE_PROJECT_NAME}
    PRIVATE fnc
)

target_link_libraries(
    ${CMAKE_PROJECT_NAME}
    PRIVATE KMTableLib::function
)


install(
	TARGETS ${CMAKE_PROJECT_NAME}
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
)
