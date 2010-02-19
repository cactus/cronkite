set(CMAKE_INCLUDE_CURRENT_DIR ON)
file(GLOB SOURCES cJSON.c)
add_library(cJSON STATIC ${SOURCES})
target_link_libraries(cJSON m)
