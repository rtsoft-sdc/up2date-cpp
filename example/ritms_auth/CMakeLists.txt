project(ritms_auth)

add_executable(${PROJECT_NAME} main.cpp)

set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)

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
