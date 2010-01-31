subdirs(cJson)
subdirs(man)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/config.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/config.h
    @ONLY)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(SOURCES main.c)
include_directories(cJson)
add_executable(cronkite ${SOURCES})
link_directories(cJson)
target_link_libraries(cronkite ${CURL_LIBRARY} cJson)
install(TARGETS cronkite DESTINATION "bin")
