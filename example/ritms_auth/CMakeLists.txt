project(ritms_auth_cli)

add_executable(${PROJECT_NAME} main.cpp)

set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)

find_package(RapidJSON CONFIG REQUIRED)

if (UNIX)
    set(LINK_FLAGS "-static-libgcc -static-libstdc++")
else()
    set(LINK_FLAGS "")
endif (UNIX)

target_link_libraries(${PROJECT_NAME}
        sub::dps
        rapidjson
        ${LINK_FLAGS}
)
