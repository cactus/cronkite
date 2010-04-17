set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(PythonLibs)
find_package(PythonInterp)
if(NOT (PYTHONLIBS_FOUND AND PYTHONINTERP_FOUND))
    MESSAGE(FATAL_ERROR "Python required to build cmodule")
endif(NOT (PYTHONLIBS_FOUND AND PYTHONINTERP_FOUND))

set(CK_INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../src)
set(CK_LIB_DIR ${CMAKE_CURRENT_BINARY_DIR}/../../src)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/setup.py.in
    ${CMAKE_CURRENT_BINARY_DIR}/setup.py @only)
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/pycronkite.c
    ${CMAKE_CURRENT_BINARY_DIR}/pycronkite.c
    COPYONLY)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/result/cronkite.so
    COMMAND ${PYTHON_EXECUTABLE} setup.py -q build --build-lib result
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/setup.py ${CMAKE_CURRENT_BINARY_DIR}/pycronkite.c cronkite
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

add_custom_target(pycronkite-cmodule ALL
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/result/cronkite.so)

## add tests
add_test(NAME t-pycronkite-cmodule
    COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/examples/python-tests/tests.py)
set_tests_properties(t-pycronkite-cmodule PROPERTIES
    ENVIRONMENT "LD_LIBRARY_PATH=${CMAKE_CURRENT_BINARY_DIR}/../../src/;DYLD_LIBRARY_PATH=${CMAKE_CURRENT_BINARY_DIR}/../../src/;CRONKITE_AURURL=file://${CMAKE_SOURCE_DIR}/examples/cronkite-cli/tests/example.json")

