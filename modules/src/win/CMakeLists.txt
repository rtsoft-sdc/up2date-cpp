# Add a library with the above sources
message("Building internal WIN HTTP client")

file(GLOB
        SOURCES
        ${PROJECT_SOURCE_DIR}/src/win/*.c
        ${PROJECT_SOURCE_DIR}/src/win/*.cpp
        ${PROJECT_SOURCE_DIR}/src/uriparse.cpp)

set(CUSTOM_HTTP_CLIENT_SRC ${SOURCES})

set(CUSTOM_HTTP_CLIENT_LINK_LIBRARIES wininet crypt32)
