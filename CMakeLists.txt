cmake_minimum_required(VERSION 2.6) 
project(cronkite C)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules ${CMAKE_MODULE_PATH})
include(MacroOutOfSourceBuild)
macro_ensure_out_of_source_build("${PROJECT_NAME} requires an out of source build.")

set(PROJECT_VERSION "0.1")
set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--hash-style=gnu -Wl,--as-needed -Wl,--no-undefined $ENV{LDFLAGS}")
set(CMAKE_C_FLAGS_DEBUG "-g -Wall -pedantic $ENV{CFLAGS}")
set(CMAKE_C_FLAGS_RELEASE "-Wall -mtune=generic -O2 -pipe $ENV{CFLAGS}")
set(CMAKE_C_FLAGS_MINSIZEREL "-Wall -mtune=generic -Os -pipe $ENV{CFLAGS}")

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE MinSizeRel CACHE STRING
      "Choose the type: None Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif(NOT CMAKE_BUILD_TYPE)

find_package(CURL)
subdirs(src)
