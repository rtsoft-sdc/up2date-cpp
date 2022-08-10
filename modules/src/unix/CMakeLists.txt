# Add a library with the above sources
message("Building internal UNIX HTTP client")

file(GLOB
        SOURCES
        ${PROJECT_SOURCE_DIR}/src/unix/*.cpp
        ${PROJECT_SOURCE_DIR}/src/uriparse.cpp)

set(CUSTOM_HTTP_CLIENT_SRC ${SOURCES})

find_package(Threads REQUIRED)
find_package(OpenSSL  COMPONENTS Crypto SSL REQUIRED)

set(CUSTOM_HTTP_CLIENT_LINK_LIBRARIES
        Threads::Threads
        OpenSSL::SSL
        OpenSSL::Crypto)
