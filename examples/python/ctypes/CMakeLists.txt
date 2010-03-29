set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../src)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../src)

find_package(PythonLibs)
find_package(PythonInterp)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/setup.py
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
