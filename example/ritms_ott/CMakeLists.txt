project(ritms_ott)

add_executable(${PROJECT_NAME} main.cpp)

if (UNIX)
    set(LINK_FLAGS "-static-libgcc -static-libstdc++")
else()
    set(LINK_FLAGS "")
endif (UNIX)

target_link_libraries(${PROJECT_NAME}
    sub::dps
    ${LINK_FLAGS}
)