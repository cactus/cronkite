set(CMAKE_INCLUDE_CURRENT_DIR ON)
find_package(CURL)
include_directories(${CURL_INCLUDE_DIRS})

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/config.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/config.h @ONLY)

# search for includes in here too
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../cJSON)
# search for libraries in here too
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../cJSON)

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
