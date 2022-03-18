project(up2date_cli)

add_executable(${PROJECT_NAME} main.cpp)

target_include_directories( ${PROJECT_NAME}
        PRIVATE ${PROJECT_SOURCE_DIR}/../include
)

target_link_libraries(${PROJECT_NAME}
        sub::ddi
        sub::dps
)