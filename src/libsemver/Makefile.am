#
# Copyright (c) 2016 Enrico M. Crisostomo
#
# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation; either version 3, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License along with
# this program.  If not, see <http://www.gnu.org/licenses/>.
#
LIBSEMVER_API_VERSION = @AM_LIBSEMVER_API_VERSION@

# Prepare gettext-related symbols used by programs
AM_CPPFLAGS = -DLOCALEDIR=\"$(localedir)\"

lib_LTLIBRARIES = libsemver.la

libsemver_la_SOURCES  = c/libsemver.cpp
libsemver_la_SOURCES += c/libsemver.h
libsemver_la_SOURCES += c++/version.hpp
libsemver_la_SOURCES += c++/version.cpp
libsemver_la_SOURCES += gettext.h
libsemver_la_SOURCES += gettext_defs.h

libsemver_la_LDFLAGS  = -version-info $(LIBSEMVER_API_VERSION)
libsemver_la_LDFLAGS += @LTLIBINTL@
