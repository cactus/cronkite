find_package(Valgrind QUIET)
if(NOT VALGRIND_FOUND)
    MESSAGE("!! Valgrind required to run memchecks. Memchecks will not be available.")
endif(NOT VALGRIND_FOUND)

find_package(PyNose QUIET)
if(NOT PYNOSE_FOUND)
    MESSAGE("!! Python-nose required to run tests. Tests will not be available.")
endif(NOT PYNOSE_FOUND)

if(NOT TARGET test)
    add_custom_target(test COMMENT "Run all tests target")
endif(NOT TARGET test)

include(ParseArguments)
macro(add_test_valgrind)
    if(VALGRIND_FOUND)
        parse_arguments(VALGRINDTEST "COMMAND;WORKING_DIRECTORY" "" ${ARGN})
        list(GET VALGRINDTEST_DEFAULT_ARGS 0 VALGRINDTEST_NAME)
        # create test target
        if(VALGRINDTEST_WORKING_DIRECTORY)
            add_custom_target(${VALGRINDTEST_NAME}
                COMMAND ${VALGRIND_EXECUTABLE}
                        --tool=memcheck --leak-check=yes 
                        --show-reachable=yes --num-callers=30
                        --leak-check=summary --leak-resolution=high
                        ${VALGRINDTEST_COMMAND}
                WORKING_DIRECTORY ${VALGRINDTEST_WORKING_DIRECTORY}
                COMMENT "Test ${inc} for ${VALGRINDTEST_NAME}")
        else(VALGRINDTEST_WORKING_DIRECTORY)
            add_custom_target(${VALGRINDTEST_NAME}
                COMMAND ${VALGRIND_EXECUTABLE}
                        --tool=memcheck --leak-check=yes 
                        --show-reachable=yes --num-callers=30
                        --leak-check=summary --leak-resolution=high
                        ${VALGRINDTEST_COMMAND}
                WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                COMMENT "Test ${inc} for ${VALGRINDTEST_NAME}")
        endif(VALGRINDTEST_WORKING_DIRECTORY)
        ## add test to main test target
        add_dependencies(test ${VALGRINDTEST_NAME})
    endif(VALGRIND_FOUND)
endmacro(add_test_valgrind)

macro(add_test_nose)
    if(PYNOSE_FOUND)
        parse_arguments(NOSETEST "WORKING_DIRECTORY;NOSEDIR" "" ${ARGN})
        list(GET NOSETEST_DEFAULT_ARGS 0 NOSETEST_NAME)
        # create test target
        if(NOSETEST_WORKING_DIRECTORY)
            add_custom_target(${NOSETEST_NAME}
                COMMAND ${PYNOSE_EXECUTABLE} ${NOSETEST_NOSEDIR} 
                COMMENT "Test ${inc} for ${NOSETEST_NAME}"
                WORKING_DIRECTORY ${NOSETEST_WORKING_DIRECTORY})
        else(NOSETEST_WORKING_DIRECTORY)
            add_custom_target(${NOSETEST_NAME}
                COMMAND ${PYNOSE_EXECUTABLE} ${NOSETEST_NOSEDIR} 
                COMMENT "Test ${inc} for ${NOSETEST_NAME}"
                WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
        endif(NOSETEST_WORKING_DIRECTORY)
        ## add test to main test target
        add_dependencies(test ${NOSETEST_NAME})
    endif(PYNOSE_FOUND)
endmacro(add_test_nose)
