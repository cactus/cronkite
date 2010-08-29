============
cronkite-cli
============

.. contents::

Requirements
------------

Build requirements:

- cmake
- gcc
- libcurl
- libcronkite
- a2x / asciidoc (required to build manpages)

Run requirements:

- libcurl
- libcronkite


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


Using
-----

Here are some examples::

    $ cronkite -search vim
    > lots  of  data
    $ cronkite -info vim
    > info  about   package vim


Environment
-----------
The following environment variables affect the execution of cronkite:

- **CRONKITE_DELIMITER**

  Specifies the delimiter/separator between the elements in the
  Output Format. Defaults to ``\t`` if not set.


Output Format
-------------
Each result in the output is emitted as a **CRONKITE_DELIMITER** separated list,
containing the following aurjson result elements, in this order::

    Name
    ID
    Version
    CategoryID
    Description
    URL
    URLPath
    License
    NumVotes
    OutOfDate


License
-------

Free use of this software is granted under the terms of the MIT (X11) License.
See LICENSE file for more information.

