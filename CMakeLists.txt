cmake_minimum_required(VERSION 2.6) 
project(cronkite C)

set(BUILD_VERSION "0.5.1")
set(API_VERSION "0.5.0")
set(PROJECT_VERSION ${BUILD_VERSION})

set(CFP "-std=c99 -Wall -pipe")
if(APPLE)
    set(CMAKE_OSX_ARCHITECTURES ppc;i386;x86_64)
    set(CMAKE_C_FLAGS_MINSIZEREL "${CFP} -Os $ENV{CFLAGS}")
    set(CMAKE_C_FLAGS_RELEASE "${CFP} -O2 $ENV{CFLAGS}")
    set(CMAKE_C_FLAGS_DEBUG "-g ${CFP} -Werror -pedantic $ENV{CFLAGS}")
else(APPLE)
    set(CMAKE_SHARED_LINKER_FLAGS 
        "-Wl,--hash-style=gnu -Wl,--as-needed -Wl,--no-undefined $ENV{LDFLAGS}")
    set(CMAKE_C_FLAGS_DEBUG "-fpic -g ${CFP} -Werror -pedantic $ENV{CFLAGS}")
    set(CMAKE_C_FLAGS_RELEASE "-fpic ${CFP} -mtune=generic -O2 $ENV{CFLAGS}")
    set(CMAKE_C_FLAGS_MINSIZEREL "-fpic ${CFP} -mtune=generic -Os $ENV{CFLAGS}")
endif(APPLE)

## set a default build type if not set
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING
      "Choose the type: Debug Release MinSizeRel." FORCE)
endif(NOT CMAKE_BUILD_TYPE)

## set module include path
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules ${CMAKE_MODULE_PATH})

## ensure out of tree build
include(MacroOutOfSourceBuild)
macro_ensure_out_of_source_build("${PROJECT_NAME} requires an out tree build.")

## import manpage build helper macros
include(BuildManpage)
## find out if we can build manpages
find_package(Pod2man)
if(POD2MAN_FOUND)
    set(MAN TRUE)
else(POD2MAN_FOUND)
    MESSAGE("!! pod2man not found. Manpage generation will not be available.")
endif(POD2MAN_FOUND)

## setup testing harness stuffs
enable_testing()
find_package(Valgrind QUIET)
if(NOT VALGRIND_FOUND)
    MESSAGE("!! Valgrind required to run memchecks. Memchecks will not be available.")
else(NOT VALGRIND_FOUND)
    set(VALGRIND_OPTIONS
        --tool=memcheck --leak-check=yes 
        --show-reachable=yes --num-callers=30
        --leak-check=summary --leak-resolution=high)
endif(NOT VALGRIND_FOUND)

## subdir inclues / conditional includes
subdirs(src)
