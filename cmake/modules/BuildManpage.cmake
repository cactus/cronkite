macro(a2x_build_manpage _depends_N _mps_N _mpt_N mversion)
    get_filename_component(_mpt_num ${_mpt_N} EXT)
    string(REPLACE "." "" _mpt_num "${_mpt_num}")
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/${_mps_N} 
         DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
         USE_SOURCE_PERMISSIONS)
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${_mpt_N}
        COMMAND ${A2X} --asciidoc-opts="-a revnumber=${mversion}" -d manpage -f manpage -D ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR}/${_mps_N}
        DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${_mps_N})
    install(
        FILES ${CMAKE_CURRENT_BINARY_DIR}/${_mpt_N}
        DESTINATION "share/man/man${_mpt_num}"
        PERMISSIONS OWNER_READ GROUP_READ WORLD_READ)
    set(${_depends_N} ${${_depends_N}} ${CMAKE_CURRENT_BINARY_DIR}/${_mpt_N})
endmacro(a2x_build_manpage _depends_N mptarg_N mpsrc_N mversion)
