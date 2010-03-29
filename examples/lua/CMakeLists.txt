set(CMAKE_INCLUDE_CURRENT_DIR ON)

if(APPLE)
    set(CMAKE_OSX_ARCHITECTURES x86_64)
endif(APPLE)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../src)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../src)

find_package(Lua51)
include_directories(${LUA_INCLUDE_DIR})

# build cronkite cli binary
add_library(luacronkite SHARED luacronkite.c)
target_link_libraries(luacronkite ${LUA_LIBRARIES} cronkite)
set_target_properties(luacronkite PROPERTIES
    OUTPUT_NAME luacronkite PREFIX "" SUFFIX ".so")

