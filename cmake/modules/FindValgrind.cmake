# Locate Valgrind
# This module defines
#  VALGRIND_FOUND
#  VALGRIND_EXECUTABLE - path to nosetests

find_program(VALGRIND_EXECUTABLE NAMES valgrind)
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Valgrind DEFAULT_MSG VALGRIND_EXECUTABLE)
MARK_AS_ADVANCED(VALGRIND_EXECUTABLE)
