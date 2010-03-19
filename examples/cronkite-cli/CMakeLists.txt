cmake_minimum_required(VERSION 2.6) 
project(cronkite-cli C)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

if(APPLE)
    set(CMAKE_OSX_ARCHITECTURES ppc;i386;x86_64)
    set(CMAKE_C_FLAGS_MINSIZEREL "-std=c99 -Wall -Os -pipe $ENV{CFLAGS}")
    set(CMAKE_C_FLAGS_RELEASE "-std=c99 -Wall -O2 -pipe $ENV{CFLAGS}")
    set(CMAKE_C_FLAGS_DEBUG "-g -Wall -Werror -std=c99 -pedantic $ENV{CFLAGS}")
else(APPLE)
    set(CMAKE_SHARED_LINKER_FLAGS 
        "-Wl,--hash-style=gnu -Wl,--as-needed -Wl,--no-undefined $ENV{LDFLAGS}")
    set(CMAKE_C_FLAGS_DEBUG "-g -Wall -Werror -std=c99 -pedantic $ENV{CFLAGS}")
    set(CMAKE_C_FLAGS_RELEASE "-std=c99 -Wall -mtune=generic -O2 -pipe $ENV{CFLAGS}")
    set(CMAKE_C_FLAGS_MINSIZEREL "-std=c99 -Wall -mtune=generic -Os -pipe $ENV{CFLAGS}")
endif(APPLE)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE MinSizeRel CACHE STRING
      "Choose the type: None Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif(NOT CMAKE_BUILD_TYPE)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/../..//cmake/modules ${CMAKE_MODULE_PATH})

include(BuildManpage)
find_program(A2X a2x)

if(NOT A2X)
    MESSAGE(WARNING "Unable to find 'a2x' from asciidoc. Skipping man build.")
else(NOT A2X)
    a2x_build_manpage(manpage_depends cronkite.1.txt cronkite.1 0)
endif(NOT A2X)

# build cronkite cli binary
add_executable(cronkite-cli cli.c)
target_link_libraries(cronkite-cli cronkite)
set_target_properties(cronkite-cli PROPERTIES OUTPUT_NAME cronkite)
# install target for cronkite cli binary
install(TARGETS cronkite-cli DESTINATION bin)
