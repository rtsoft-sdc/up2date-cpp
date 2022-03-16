cmake_minimum_required (VERSION 3.5)

# devcontainer support
if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
            CACHE STRING "")
endif()

project(up2date-cpp)

# Add sub directories
add_subdirectory(modules)
add_subdirectory(hawkbit)
add_subdirectory(example)
