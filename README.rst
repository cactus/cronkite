cronkite
========

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

- NOGZIP = ``OFF``
  Whether to disable compressing manpages with gzip. 
  Turn this on if you wish to disable gzip manpage compression,
  as some packaging utilities will do the gzip compression step for you.

- CMAKE_INSTALL_PREFIX = ``/usr/local``
  The target prefix that make install will install to.
  This is what you would expect from ``./configure --prefix=foo``

- CMAKE_BUILD_TYPE = ``MinSizeRel``
  Specifies the build type for make based generators.
  Possible values: Debug, Release, RelWithDebInfo, MinSizeRel

The build steps are::

    $ mkdir build
    $ cd build
    $ cmake -DNOGZIP=OFF -DCMAKE_INSTALL_PREFIX=/usr/local ..
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

License
-------

Apache License, Version 2.0   
http://www.apache.org/licenses/LICENSE-2.0

