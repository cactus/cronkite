set(CMAKE_INCLUDE_CURRENT_DIR ON)
file(GLOB SOURCES *.c)
add_library(jansson STATIC ${SOURCES})
