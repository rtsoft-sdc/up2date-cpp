# Add a library with the above sources
message("Building internal WIN HTTP client")

file(GLOB
        SOURCES
        ${PROJECT_SOURCE_DIR}/src/win/win_http.c
        ${PROJECT_SOURCE_DIR}/src/win/converters.c
        ${PROJECT_SOURCE_DIR}/src/win/httpclient_impl.cpp
        ${PROJECT_SOURCE_DIR}/src/uriparse.cpp)


set(CUSTOM_HTTP_CLIENT_SRC ${SOURCES})

set(CUSTOM_HTTP_CLIENT_LINK_LIBRARIES winhttp crypt32)
