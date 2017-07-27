[![License](https://img.shields.io/badge/license-GPL--3.0-blue.svg?style=flat)](https://github.com/emcrisostomo/semver-utils/blob/master/LICENSE)

README
======

A C/C++ library and an utility to parse, compare and manipulate versions numbers
complying with [Semantic Versioning 2.0.0][semver].

[semver]: http://semver.org/

Table of Contents
-----------------

  * [Features](#features)
  * [Building from Source](#building-from-source)
  * [Installation](#installation)
  * [Documentation](#documentation)
  * [Localization](#localization)
  * [Usage](#usage)
  * [Contributing](#contributing)
  * [Bug Reports](#bug-reports)

Features
--------

`libsemver` features are the following:

  * Version parsing and splitting into their components.

  * Version comparison: <, == and > are supported.

  * Version bump, per coordinate.

`semver` is a program built on top of `libsemver` providing the following
functionality:

  * All the functionality provided by `libsemver`.

  * Sorting of versions.

Building from Source
--------------------

A user who wishes to build `libsemver` should get a [release tarball][release].
A release tarball contains everything a user needs to build `libsemver` on their
system, following the instructions detailed in the [Installation](#installation)
section below and the `INSTALL` file.

A developer who wishes to modify `libsemver` should get the sources (either from
a source tarball or cloning the repository) and have the GNU Build System
installed on their machine.  Please read `README.gnu-build-system` to get
further details about how to bootstrap `libsemver` from sources on your machine.

Getting a copy of the source repository is not recommended unless you are a
developer, you have the GNU Build System installed on your machine, and you know
how to bootstrap it on the sources.

[release]: https://github.com/emcrisostomo/semver-utils/releases

Installation
------------

See the `INSTALL` file for detailed information about how to configure and
install this package.  Since `semver` builds and uses dynamic libraries, in some
platforms you may need to perform additional tasks before you can use it:

  * Make sure the installation directory of dynamic libraries (`$PREFIX/lib`) is
    included in the lookup paths of the dynamic linker of your operating system.
    The default path, `/usr/local/lib`, will work in nearly every operating
    system.

  * Refreshing the links and cache to the dynamic libraries may be required.  In
    GNU/Linux systems you may need to run `ldconfig`:

        $ ldconfig

`libsemver` is a C++ program and a C++ compiler compliant with the C++11 standard
is required to compile it.  Check your OS documentation for information about
how to install the C++ toolchain and the C++ runtime.

Documentation
-------------

`libsemver` provides the following documentation:

  * HTML documentation.
  * PDF documentation.


Localization
------------

`libsemver` is localizable and internally it uses GNU `gettext` to decouple
localizable string from their translation.  The currently available locales are:

  * English (`en`).
  * Italian (`it`).
  * Spanish (`es`).

To build `libsemver` with localization support, you need to have `gettext`
installed on your system.  If `configure` cannot find `<libintl.h>` or the
linker cannot find `libintl`, then you may need to manually provide their
location to `configure`, usually using the `CPPFLAGS` and the `LDFLAGS`
variables.  See `README.osx` for an example.

If `gettext` is not available on your system, `libsemver` shall build correctly,
but it will lack localization support and the only available locale will be
English.

Usage
-----

`semver` accepts a list of versions as parameters and the operation performed on
them depends on the command being executed.

    $ semver command [options] ... version-0 ... version-n

The maximum number of accepted arguments depends on the `command` being
executed.

`command` may be one of the following:

  * `-b`: versions are _bumped_.

        $ semver -b major 1.3.4
        2.0.0
        $ semver -b minor 1.3.4
        1.4.0
        $ semver -b patch 1.3.4
        1.3.5

  * `-c`: versions are _compared_.

        $ semver -c 1.3.4 2.3.0
        $ echo $?
        1
        $ semver -c 2.3.0 1.3.4
        $ echo $?
        2
        $ semver -c 2.3.0 2.3.0
        $ echo $?
        0

  * `-s`: versions are _sorted_.

        $ semver -s 1.0.0 1.0.0-rc.1 1.0.0-beta.11 1.0.0-beta.2 \
                    1.0.0-beta 1.0.0-alpha.beta 1.0.0-alpha.1 1.0.0-alpha
        1.0.0-alpha
        1.0.0-alpha.1
        1.0.0-alpha.beta
        1.0.0-beta
        1.0.0-beta.2
        1.0.0-beta.11
        1.0.0-rc.1
        1.0.0

  * `-v`: versions are _verified_.

        $ semver -v 1.3.5-develop+x86
        $ echo $?
        0
        $ semver -v 1.3.5-develop.03
        Numerical identifier cannot contain leading zeroes.
        $ echo $?
        1

For more information, refer to the `semver` documentation or the output of the
following command:

    $ semver --help

Contributing
------------

Everybody is welcome to contribute to `libsemver`.  Please, see
[`CONTRIBUTING`][contrib] for further information.

[contrib]: CONTRIBUTING.md

Bug Reports
-----------

Bug reports can be reported as issues on the [project home page][semver-utils].

[semver-utils]: https://github.com/emcrisostomo/semver-utils

-----

Copyright (c) 2016-2017 Enrico M. Crisostomo

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation; either version 3, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program.  If not, see <http://www.gnu.org/licenses/>.
