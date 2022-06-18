project(wrapperdll)
include (GenerateExportHeader)

set (CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
add_library(wrapperdll SHARED dllmain.cpp ddiclient.cpp CallbackDispatcher.cpp) 
GENERATE_EXPORT_HEADER( wrapperdll
             BASE_NAME wrapperdll
             EXPORT_MACRO_NAME WDLL_EXPORT
             STATIC_DEFINE WDLL_BUILT_AS_STATIC
             EXPORT_FILE_NAME ${PROJECT_SOURCE_DIR}/dllexport.h
)

target_include_directories( ${PROJECT_NAME}
        PRIVATE ${PROJECT_SOURCE_DIR}
)

target_link_libraries(${PROJECT_NAME}
        sub::ddi
        sub::dps
)