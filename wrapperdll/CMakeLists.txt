cmake_minimum_required(VERSION 3.23)
project(wrapperdll)
include (GenerateExportHeader)

add_library(wrapperdll SHARED dllmain.cpp ddiclient.cpp) 
GENERATE_EXPORT_HEADER( wrapperdll
             BASE_NAME wrapperdll
             EXPORT_MACRO_NAME WDLL_EXPORT
             EXPORT_FILE_NAME WDLL_Export.h
             STATIC_DEFINE WDLL_BUILT_AS_STATIC
)

target_include_directories( ${PROJECT_NAME}
        PRIVATE ${PROJECT_SOURCE_DIR}
)

target_link_libraries(${PROJECT_NAME}
        sub::ddi
)