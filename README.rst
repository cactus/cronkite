========
cronkite
========

.. contents::

Requirements
------------

Build requirements:

- cmake
- gcc
- libcurl

Optional build requirements:

- a2x / asciidoc (required to build manpages)
- lua (required for building the lua extension)
- python (required for building python extensions, and running tests)
- python-nose (required to run tests)

Include 3rd party libraries:

- cJSON_ (src/cJSON)

Run requirements:

- libcurl
- lua (lua extension only)
- python (python extensions only)

.. _cJSON: http://sourceforge.net/projects/cjson/


Building
--------

There are a few build options, and their defaults, that you can specify:

- **CMAKE_INSTALL_PREFIX** = ``/usr/local``

  The target prefix that make install will install to.
  This is what you would expect from ``./configure --prefix=foo``

- **CMAKE_BUILD_TYPE** = ``MinSizeRel``

  Specifies the build type for make based generators.
  Possible values: Debug, Release, RelWithDebInfo, MinSizeRel

- **BUILD_LUA_MODULE** = ``OFF``

  Build the lua module (luacronkite) extension.

- **BUILD_PYTHON_CMODULE** = ``OFF``

  Build python cmodule style extension (pycronkite.py).

- **BUILD_PYTHON_CTYPES** = ``OFF``

  Build python ctypes style extension (cronkite.so).

- **BUILD_EXAMPLE_CLI** = ``OFF``

  Build the example cli frontend (cronkite-cli).

- **BUILD_ALL** = ``OFF``

  Build everything -- all modules and frontends.

The build steps are::

    $ mkdir build
    $ cd build
    $ cmake -DCMAKE_INSTALL_PREFIX=/usr/local ..
    $ make


Installing
----------

After building, while in the build directory::

    $ make install


License
-------

Apache License, Version 2.0 <http://www.apache.org/licenses/LICENSE-2.0>

