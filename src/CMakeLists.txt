set(CMAKE_INCLUDE_CURRENT_DIR ON)

# process subdirs
subdirs(cJSON)
subdirs(man)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/config.h.in
    config.h @ONLY)

# search for includes in here too
include_directories(cJSON)
# search for libraries in here too
link_directories(cJSON)

# build libcronkite
add_library(cronkite SHARED cronkite.c)
target_link_libraries(cronkite ${CURL_LIBRARY} cJSON)
set_target_properties(cronkite PROPERTIES
    VERSION ${BUILD_VERSION}
    SOVERSION ${API_VERSION}
    PUBLIC_HEADER cronkite.h)
install(TARGETS cronkite
    LIBRARY DESTINATION lib
    PUBLIC_HEADER DESTINATION include)
