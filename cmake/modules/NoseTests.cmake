include(ParseArguments)
macro(nose_tests_init)
    find_package(PythonInterp QUIET)
    if(NOT PYTHONINTERP_FOUND)
        MESSAGE(WARNING "Python required to run tests")
    else(NOT PYTHONINTERP_FOUND)
        find_package(PyNose QUIET)
        if(NOT PYNOSE_FOUND)
            MESSAGE(WARNING "Python-nose required to run tests")
        else(NOT PYNOSE_FOUND)
            add_custom_target(tests COMMENT "Run all tests target")
        endif(NOT PYNOSE_FOUND)
    endif(NOT PYTHONINTERP_FOUND)
endmacro(nose_tests_init)
macro(nose_add_test)
    if(PYNOSE_FOUND)
        parse_arguments(NOSETEST "WORKING_DIRECTORY" "" ${ARGN})
        set(NT_ARGS ${NOSETEST_DEFAULT_ARGS})
        list(GET NT_ARGS 0 NOSETEST_NAME)
        list(REMOVE_AT NT_ARGS 0)
        # create test target
        add_custom_target(${NOSETEST_NAME} COMMENT "${NOSETEST_NAME} tests")
        set(inc 0)
        foreach(farg ${NT_ARGS})
            math(EXPR inc "${inc} + 1")
            if(NOSETEST_WORKING_DIRECTORY)
                add_custom_target(${NOSETEST_NAME}_${inc}
                    COMMAND ${PYNOSE_EXECUTABLE} ${farg} 
                    COMMENT "Test ${inc} for ${NOSETEST_NAME}"
                    WORKING_DIRECTORY ${NOSETEST_WORKING_DIRECTORY})
            else(NOSETEST_WORKING_DIRECTORY)
                add_custom_target(${NOSETEST_NAME}_${inc}
                    COMMAND ${PYNOSE_EXECUTABLE} ${farg} 
                    COMMENT "Test ${inc} for ${NOSETEST_NAME}")
            endif(NOSETEST_WORKING_DIRECTORY)
            ## add tests to test target
            add_dependencies(${NOSETEST_NAME} ${NOSETEST_NAME}_${inc})
        endforeach(farg)
        ## add test to main test target
        add_dependencies(tests ${NOSETEST_NAME})
    endif(PYNOSE_FOUND)
endmacro(nose_add_test)
