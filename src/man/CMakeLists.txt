if(MAN)
    p2m_build(cli-manpage_depends ${CMAKE_CURRENT_SOURCE_DIR}/cronkite-cli.1.pod ${BUILD_VERSION})
    add_custom_target(man-cronkite-cli ALL DEPENDS ${cli-manpage_depends})

    p2m_build(manpage_depends ${CMAKE_CURRENT_SOURCE_DIR}/cronkite.3.pod ${API_VERSION})
    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/cronkite_seturl.3
        ".so man3/cronkite.3\n")
    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/cronkite_get.3
        ".so man3/cronkite.3\n")
    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/cronkite_cleanup.3
        ".so man3/cronkite.3\n")
    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/cronkite_strerror.3
        ".so man3/cronkite.3\n")
    ## prebuild manpages
    install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/cronkite_seturl.3
        ${CMAKE_CURRENT_BINARY_DIR}/cronkite_get.3
        ${CMAKE_CURRENT_BINARY_DIR}/cronkite_cleanup.3
        ${CMAKE_CURRENT_BINARY_DIR}/cronkite_strerror.3
        DESTINATION "share/man/man3"
        PERMISSIONS OWNER_READ GROUP_READ WORLD_READ)
    add_custom_target(man-cronkite ALL
        DEPENDS ${manpage_depends}
            ${CMAKE_CURRENT_BINARY_DIR}/cronkite_seturl.3
            ${CMAKE_CURRENT_BINARY_DIR}/cronkite_get.3
            ${CMAKE_CURRENT_BINARY_DIR}/cronkite_cleanup.3
            ${CMAKE_CURRENT_BINARY_DIR}/cronkite_strerror.3)
endif(MAN)
