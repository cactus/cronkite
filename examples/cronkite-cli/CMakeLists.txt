set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../src)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../src)

if(A2X)
    a2x_build_manpage(cli-manpage_depends 
        cronkite-cli.1.txt cronkite-cli.1 ${BUILD_VERSION})
    add_custom_target(cli-manpage ALL DEPENDS ${cli-manpage_depends})
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

nose_add_test(cli-tests 
    ${CMAKE_CURRENT_SOURCE_DIR}/tests.py 
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

