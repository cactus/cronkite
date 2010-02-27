set(CMAKE_INCLUDE_CURRENT_DIR ON)
add_library(cJSON STATIC cJSON.c)
target_link_libraries(cJSON m)
