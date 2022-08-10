cmake_minimum_required (VERSION 3.5)
project(up2date-cpp)

# -- HTTP_CLIENT implementation type --
set(U2D_DEFAULT_HTTP_CLIENT_IMPL UNIX WIN ESP32)
set(U2D_SUPPORTED_HTTP_CLIENT_IMPL ${U2D_DEFAULT_HTTP_CLIENT_IMPL} CUSTOM AUTO)

set(U2D_HTTP_CLIENT_IMPL "AUTO" CACHE STRING "up2date http client implementation")
set_property(CACHE U2D_HTTP_CLIENT_IMPL PROPERTY STRINGS ${U2D_SUPPORTED_HTTP_CLIENT_IMPL})

list(FIND U2D_SUPPORTED_HTTP_CLIENT_IMPL ${U2D_HTTP_CLIENT_IMPL} index)
if(index EQUAL -1)
    message(FATAL_ERROR "U2D_HTTP_CLIENT_IMPL must be one of ${U2D_SUPPORTED_HTTP_CLIENT_IMPL}")
endif()

if (${U2D_HTTP_CLIENT_IMPL} STREQUAL "AUTO")
    if (WIN32)
        set(_HTTP_CLI_BUILD_WIN ON CACHE INTERNAL "")
    elseif(UNIX)
        set(_HTTP_CLI_BUILD_UNIX ON CACHE INTERNAL "")
    else()
        message(FATAL_ERROR "Cannot detect target system for U2D_HTTP_CLIENT_IMPL=AUTO. Only UNIX/WIN32 supported")
    endif()
elseif(${U2D_HTTP_CLIENT_IMPL} STREQUAL UNIX)
    set(_HTTP_CLI_BUILD_UNIX ON CACHE INTERNAL "")
elseif(${U2D_HTTP_CLIENT_IMPL} STREQUAL WIN)
    set(_HTTP_CLI_BUILD_WIN ON CACHE INTERNAL "")
elseif(${U2D_HTTP_CLIENT_IMPL} STREQUAL ESP32)
    set(_HTTP_CLI_BUILD_ESP32 ON CACHE INTERNAL "")
elseif(${U2D_HTTP_CLIENT_IMPL} STREQUAL CUSTOM)
    message("Using custom mode. Check variables CUSTOM_HTTP_CLIENT_SRC CUSTOM_HTTP_CLIENT_LINK_LIBRARIES CUSTOM_HTTP_CLIENT_INCLUDE_DIRS is set")
endif()
# -- + --

option(BUILD_EXAMPLES "Build examples" OFF)
option(BUILD_DPS "Build module for communication with RITMS provisioning service" ON)


# Add sub directories
# - external dependencies
add_subdirectory(deps)
# - shared modules
add_subdirectory(modules)
# - ddi api
add_subdirectory(ddi)
# - ritms up2date communication api
if (BUILD_DPS)
    add_subdirectory(dps)
endif()

if (BUILD_EXAMPLES)
    add_subdirectory(example)
endif()
