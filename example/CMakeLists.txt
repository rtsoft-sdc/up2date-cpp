project(hawkbit_example_cli)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME}
    sub::hawkbitl
)