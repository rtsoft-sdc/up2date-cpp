project(example)

add_subdirectory(standalone_client)

if (BUILD_DPS)
    add_subdirectory(up2date_client)
    add_subdirectory(ritms_auth)
    add_subdirectory(ritms_ott)
endif()
