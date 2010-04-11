cmake_minimum_required(VERSION 2.6) 
project(cronkite C)

set(BUILD_VERSION "0.4.2")
set(API_VERSION "0.4.0")
set(PROJECT_VERSION ${BUILD_VERSION})

if(APPLE)
    set(CMAKE_OSX_ARCHITECTURES ppc;i386;x86_64)
    set(CMAKE_C_FLAGS_MINSIZEREL "-std=c99 -Wall -Os -pipe $ENV{CFLAGS}")
    set(CMAKE_C_FLAGS_RELEASE "-std=c99 -Wall -O2 -pipe $ENV{CFLAGS}")
    set(CMAKE_C_FLAGS_DEBUG "-g -Wall -Werror -std=c99 -pedantic $ENV{CFLAGS}")
else(APPLE)
    set(CMAKE_SHARED_LINKER_FLAGS 
        "-Wl,--hash-style=gnu -Wl,--as-needed -Wl,--no-undefined $ENV{LDFLAGS}")
    set(CMAKE_C_FLAGS_DEBUG "-fpic -g -Wall -Werror -std=c99 -pedantic $ENV{CFLAGS}")
    set(CMAKE_C_FLAGS_RELEASE "-fpic -std=c99 -Wall -mtune=generic -O2 -pipe $ENV{CFLAGS}")
    set(CMAKE_C_FLAGS_MINSIZEREL "-fpic -std=c99 -Wall -mtune=generic -Os -pipe $ENV{CFLAGS}")
endif(APPLE)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules ${CMAKE_MODULE_PATH})
include(MacroOutOfSourceBuild)
include(NoseTests)
macro_ensure_out_of_source_build("${PROJECT_NAME} requires an out tree build.")

## set a default build type
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE MinSizeRel CACHE STRING
      "Choose the type: None Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif(NOT CMAKE_BUILD_TYPE)

## find out if we can build manpages
find_program(A2X a2x)
if(NOT A2X)
    MESSAGE(WARNING "Unable to find 'a2x' from asciidoc. Skipping man builds.")
else(NOT A2X)
    include(BuildManpage)
endif(NOT A2X)

## setup testing harness stuffs
nose_tests_init()

## subdir inclues / conditional includes
subdirs(src)

option(BUILD_EXAMPLE_CLI "Build example cronkite cli" OFF)
if(BUILD_EXAMPLE_CLI)
    subdirs(examples/cronkite-cli/)
endif(BUILD_EXAMPLE_CLI)

option(BUILD_LUA_MODULE "Build lua module extension" OFF)
if(BUILD_LUA_MODULE)
    subdirs(examples/lua/)
endif(BUILD_LUA_MODULE)

option(BUILD_PYTHON_CMODULE "Build python cmodule style extension" OFF)
if(BUILD_PYTHON_CMODULE)
    subdirs(examples/python-cmodule/)
endif(BUILD_PYTHON_CMODULE)

option(BUILD_PYTHON_CTYPES "Build python ctypes style extension" OFF)
if(BUILD_PYTHON_CTYPES)
    subdirs(examples/python-ctypes/)
endif(BUILD_PYTHON_CTYPES)
