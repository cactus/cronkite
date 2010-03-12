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
- a2x / asciidoc (required to build manpages)

Run requirements:

- libcurl


Building
--------

There are a few build options, and their defaults, that you can specify:

- **CMAKE_INSTALL_PREFIX** = ``/usr/local``

  The target prefix that make install will install to.
  This is what you would expect from ``./configure --prefix=foo``

- **CMAKE_BUILD_TYPE** = ``MinSizeRel``

  Specifies the build type for make based generators.
  Possible values: Debug, Release, RelWithDebInfo, MinSizeRel

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

