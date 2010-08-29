========
cronkite
========

.. contents::

Requirements
------------

Build requirements:

- gcc
- cmake
- make
- libcurl

Optional build requirements:

- pod2man (required to build manpages)
- valgrind (required to run memory leak test)

Included 3rd party libraries:

- cJSON_ (src/cJSON)

Run requirements:

- libcurl

.. _cJSON: http://sourceforge.net/projects/cjson/


Building
--------

There are a few build options, and their defaults, that you can specify:

- **CMAKE_INSTALL_PREFIX** = ``/usr/local``

  | The target prefix that make install will install to.
  | This is what you would expect from ``./configure --prefix=foo``

- **CMAKE_BUILD_TYPE** = ``Release``

  | Specifies the build type for make based generators.
  | Possible values: Debug, Release, MinSizeRel

An example of the build steps::

    $ mkdir build
    $ cd build
    $ cmake -DCMAKE_INSTALL_PREFIX=/usr/local ..
    $ make


Running Tests
-------------

To run the tests, do the following after the build phase::

    $ make test


Installing
----------

After building, while in the build directory::

    $ make install


Docs and Examples
-----------------

Documentation in the form of manpages exists for both the library, and for the
example cli application:

- cronkite(3) - manpage for the libcronkite exposed functions.
- cronkite(1) - manpage for the example cli application.

To get more information about using the library, see the manpages, and/or
reference the example cli application code (included in src/cli/).

License
-------

Free use of this software is granted under the terms of the MIT (X11) License.
See LICENSE file for more information.
