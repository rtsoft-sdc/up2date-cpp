project(modules)

add_library("interface-${PROJECT_NAME}" INTERFACE)
target_include_directories("interface-${PROJECT_NAME}" INTERFACE "${PROJECT_SOURCE_DIR}/include")

if(DEFINED _HTTP_CLI_BUILD_UNIX)
    include(src/unix/CMakeLists.txt)
elseif(DEFINED _HTTP_CLI_BUILD_WIN)
    include(src/win/CMakeLists.txt)
elseif(DEFINED _HTTP_CLI_BUILD_WIN_OPENSSL)
    include(src/win_openssl/CMakeLists.txt)
else()
    message("Building custom HTTP client")
endif()


add_library(${PROJECT_NAME} ${CUSTOM_HTTP_CLIENT_SRC})
add_library(sub::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)

if(DEFINED CUSTOM_HTTP_CLIENT_INCLUDE_DIRS)
    target_include_directories(${PROJECT_NAME}
            PRIVATE
            ${CUSTOM_HTTP_CLIENT_INCLUDE_DIRS})
endif()

target_link_libraries(${PROJECT_NAME}
        PUBLIC
        "interface-${PROJECT_NAME}"
        PRIVATE
        ${CUSTOM_HTTP_CLIENT_LINK_LIBRARIES})