include(BuildManpage)
find_program(A2X a2x)

if(NOT A2X)
    MESSAGE(WARNING "Unable to find 'a2x' from asciidoc. Skipping man build.")
else(NOT A2X)
    a2x_build_manpage(manpage_depends
        libcronkite.3.txt libcronkite.3 ${BUILD_VERSION})
    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/cronkite_get.3
        ".so man3/libcronkite.3\n")
    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/cronkite_cleanup.3
        ".so man3/libcronkite.3\n")
    ## prebuild manpages
    install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/cronkite_get.3
        ${CMAKE_CURRENT_BINARY_DIR}/cronkite_cleanup.3
        DESTINATION "share/man/man3"
        PERMISSIONS OWNER_READ GROUP_READ WORLD_READ)
    add_custom_target(manpage ALL
        DEPENDS ${manpage_depends}
            ${CMAKE_CURRENT_BINARY_DIR}/cronkite_get.3
            ${CMAKE_CURRENT_BINARY_DIR}/cronkite_cleanup.3)
endif(NOT A2X)
