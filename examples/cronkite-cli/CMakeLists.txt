set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../src)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../src)

if(A2X)
    a2x_build_manpage(manpage_depends cronkite.1.txt cronkite.1 0)
endif(A2X)

# build cronkite cli binary
add_executable(cronkite-cli cli.c)
target_link_libraries(cronkite-cli cronkite)
set_target_properties(cronkite-cli PROPERTIES OUTPUT_NAME cronkite)
# install target for cronkite cli binary
install(TARGETS cronkite-cli DESTINATION bin)
