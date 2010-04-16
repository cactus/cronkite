cmake_minimum_required(VERSION 2.6) 
project(cronkite C)

set(BUILD_VERSION "0.4.4")
set(API_VERSION "0.4.0")
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

## find out if we can build manpages
find_program(A2X a2x)
if(NOT A2X)
    MESSAGE("!! a2x not found. Manpage generation will not be available.")
else(NOT A2X)
    include(BuildManpage)
endif(NOT A2X)

## subdir inclues / conditional includes
subdirs(src)

option(BUILD_ALL "Build all modules and examples" OFF)

option(BUILD_EXAMPLE_CLI "Build example cronkite cli" OFF)
if(BUILD_ALL OR BUILD_EXAMPLE_CLI)
    subdirs(examples/cronkite-cli/)
endif(BUILD_ALL OR BUILD_EXAMPLE_CLI)

option(BUILD_LUA_MODULE "Build lua module extension" OFF)
if(BUILD_ALL OR BUILD_LUA_MODULE)
    subdirs(examples/lua/)
endif(BUILD_ALL OR BUILD_LUA_MODULE)

option(BUILD_PYTHON_CMODULE "Build python cmodule style extension" OFF)
if(BUILD_ALL OR BUILD_PYTHON_CMODULE)
    find_package(PyNose QUIET)
    if(NOT PYNOSE_FOUND)
        set(NONOSE TRUE)
        MESSAGE("!! Python-nose required to run python module tests.")
    endif(NOT PYNOSE_FOUND)
    subdirs(examples/python-cmodule/)
endif(BUILD_ALL OR BUILD_PYTHON_CMODULE)

option(BUILD_PYTHON_CTYPES "Build python ctypes style extension" OFF)
if(BUILD_ALL OR BUILD_PYTHON_CTYPES)
    if(NOT NONOSE AND NOT PYNOSE_FOUND)
        find_package(PyNose QUIET)
        if(NOT PYNOSE_FOUND)
            MESSAGE("!! Python-nose required to run python module tests.")
        endif(NOT PYNOSE_FOUND)
    endif(NOT NONOSE AND NOT PYNOSE_FOUND)
    subdirs(examples/python-ctypes/)
endif(BUILD_ALL OR BUILD_PYTHON_CTYPES)
