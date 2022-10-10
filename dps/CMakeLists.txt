project (dps LANGUAGES CXX)


# Add a library with the above sources
file(GLOB SOURCES "src/*.cpp")
add_library(${PROJECT_NAME} ${SOURCES})
add_library(sub::dps ALIAS ${PROJECT_NAME})

set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories( ${PROJECT_NAME}
        PUBLIC ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries( ${PROJECT_NAME}
        PRIVATE sub::modules
        rapidjson
)