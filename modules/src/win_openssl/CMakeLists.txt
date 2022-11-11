# Add a library with the above sources
message("Building WIN(OPENSSL) HTTP client")

file(GLOB
        SOURCES
        ${PROJECT_SOURCE_DIR}/src/unix/*.cpp
        ${PROJECT_SOURCE_DIR}/src/uriparse.cpp)

set(CUSTOM_HTTP_CLIENT_SRC ${SOURCES})

find_package(Threads REQUIRED)

if (${_CLI_BUILD_OPENSSL})
    set(_OPENSSL_COMPONENTS ssl crypto)
else()
    find_package(OpenSSL  COMPONENTS Crypto SSL REQUIRED)
    set(_OPENSSL_COMPONENTS OpenSSL::SSL OpenSSL::Crypto)
endif()


set(CUSTOM_HTTP_CLIENT_LINK_LIBRARIES
        Threads::Threads
        ws2_32
        crypt32
        cryptui
        ${_OPENSSL_COMPONENTS})
