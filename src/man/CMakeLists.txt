option(NOGZIP "Disable GZIP of manpages" OFF)
if(NOT NOGZIP)
    find_program(GZIP gzip)
    if(NOT GZIP)
         MESSAGE(WARNING "Could not find 'gzip'. Not compression manpages.")
    endif(NOT GZIP)
endif(NOT NOGZIP)

find_program(A2X a2x)
if(NOT A2X)
    MESSAGE(FATAL_ERROR "Unable to find 'a2x' from asciidoc")
endif(NOT A2X)

if(A2X)
    set(MANPAGE cronkite.1)
    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/${MANPAGE}.txt 
        ${CMAKE_CURRENT_BINARY_DIR}/${MANPAGE}.txt.out @ONLY)
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${MANPAGE}
        COMMAND ${A2X} -d manpage -f manpage -D ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR}/${MANPAGE}.txt.out
        DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${MANPAGE}.txt.out)
    if(GZIP)
        add_custom_command(
            OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${MANPAGE}.gz
            COMMAND ${GZIP} -c ${CMAKE_CURRENT_BINARY_DIR}/${MANPAGE} > ${CMAKE_CURRENT_BINARY_DIR}/${MANPAGE}.gz
            DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${MANPAGE})
        set(MANPAGE ${MANPAGE}.gz)
    endif(GZIP)
    add_custom_target(
        manpage ALL
        DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${MANPAGE})
    install(
        FILES ${CMAKE_CURRENT_BINARY_DIR}/${MANPAGE}
        DESTINATION "share/man/man1"
        PERMISSIONS OWNER_READ GROUP_READ WORLD_READ)
endif(A2X)

