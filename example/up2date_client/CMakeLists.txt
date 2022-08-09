project(up2date_client)

add_executable(${PROJECT_NAME} main.cpp)

target_include_directories( ${PROJECT_NAME}
        PRIVATE ${PROJECT_SOURCE_DIR}/../include
)

if (UNIX)
    set(LINK_FLAGS "-static-libgcc -static-libstdc++")
else()
    set(LINK_FLAGS "")
endif (UNIX)

target_link_libraries(${PROJECT_NAME}
        sub::ddi
        sub::dps
        ${LINK_FLAGS}
)