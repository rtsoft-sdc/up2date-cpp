project (hawkbit LANGUAGES CXX)


# Add a library with the above sources
file(GLOB SOURCES "src/*.cpp")
add_library(${PROJECT_NAME} ${SOURCES})
add_library(sub::hawkbitl ALIAS ${PROJECT_NAME})

set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories( ${PROJECT_NAME}
    PUBLIC ${PROJECT_SOURCE_DIR}/include
)

target_include_directories( ${PROJECT_NAME}
    PRIVATE ${PROJECT_SOURCE_DIR}/submodules/rapidjson/include
)


find_package(Threads REQUIRED)
find_package(OpenSSL  COMPONENTS Crypto SSL REQUIRED)

target_link_libraries( ${PROJECT_NAME}
        Threads::Threads
        $<$<PLATFORM_ID:Windows>:ws2_32>
        $<$<PLATFORM_ID:Windows>:crypt32>
        $<$<PLATFORM_ID:Windows>:cryptui>
        OpenSSL::SSL
        OpenSSL::Crypto
)

