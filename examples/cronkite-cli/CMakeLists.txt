set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../src)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../src)

if(A2X)
    a2x_build_manpage(cli-manpage_depends 
        cronkite-cli.1.txt cronkite-cli.1 ${BUILD_VERSION})
    add_custom_target(cronkite-cli-man ALL DEPENDS ${cli-manpage_depends})
endif(A2X)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cli.h.in
    cli.h @ONLY)

# build cronkite cli binary
add_executable(cronkite-cli cli.c)
target_link_libraries(cronkite-cli cronkite)
set_target_properties(cronkite-cli PROPERTIES OUTPUT_NAME cronkite-cli)
# install target for cronkite cli binary
install(TARGETS cronkite-cli DESTINATION bin)

add_test_nose(test-cronkite-cli NOSEDIR ${CMAKE_CURRENT_SOURCE_DIR}/tests/)

#if(PYNOSE_FOUND)
#    add_test(NAME cli-tests-nose 
#        COMMAND ${PYNOSE_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tests.py)
#endif(PYNOSE_FOUND)

if(VALGRIND_FOUND)
    add_test_valgrind(memcheck-cronkite-cli
        COMMAND ${CMAKE_CURRENT_BINARY_DIR}/cronkite-cli -search example)
endif(VALGRIND_FOUND)

