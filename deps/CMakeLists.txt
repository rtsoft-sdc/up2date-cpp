# === RapidJson block ===
if (${_CLI_BUILD_OPENSSL})
    set(WITH_APPS OFF CACHE INTERNAL "")
    add_subdirectory(openssl-cmake)
endif()

add_library(rapidjson INTERFACE)
target_include_directories(rapidjson INTERFACE rapidjson/include)
# === + ===