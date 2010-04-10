set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(PythonLibs)
find_package(PythonInterp)
if(NOT (PYTHONLIBS_FOUND AND PYTHONINTERP_FOUND))
    MESSAGE(FATAL_ERROR "Python required to build cmodule")
endif(NOT (PYTHONLIBS_FOUND AND PYTHONINTERP_FOUND))

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/setup.py.in
    ${CMAKE_CURRENT_BINARY_DIR}/setup.py @only)
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/pycronkite.py
    ${CMAKE_CURRENT_BINARY_DIR}/pycronkite.py
    COPYONLY)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/result/pycronkite.py
    COMMAND ${PYTHON_EXECUTABLE} setup.py -q build_py --build-lib result
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/setup.py ${CMAKE_CURRENT_BINARY_DIR}/pycronkite.py
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

add_custom_target(pycronkite-ctypes ALL
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/result/pycronkite.py)
