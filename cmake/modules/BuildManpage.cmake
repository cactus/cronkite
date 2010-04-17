macro(p2m_build _depends_N _p_input mversion)
    if(POD2MAN_FOUND)
        get_filename_component(_p_tmp ${_p_input} EXT)
        get_filename_component(_oname ${_p_input} NAME_WE)
        # should be something like .1.pod now
        string(SUBSTRING ${_p_tmp} 1 1 _m_num)
        set(ofile ${CMAKE_CURRENT_BINARY_DIR}/${_oname}.${_m_num})

        add_custom_command(
            OUTPUT ${ofile}
            COMMAND ${POD2MAN_EXECUTABLE}
                -s ${_m_num} -n ${_oname}
                -c ${_oname} -r "${mversion}" ${_p_input} ${ofile})
        install(
            FILES ${ofile}
            DESTINATION "share/man/man${_m_num}"
            PERMISSIONS OWNER_READ GROUP_READ WORLD_READ)
        set(${_depends_N} ${${_depends_N}} ${ofile})
    endif(POD2MAN_FOUND)
endmacro(p2m_build _depends_N _p_input mversion)
