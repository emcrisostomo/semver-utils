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
# Prepare gettext-related symbols used by programs
bin_PROGRAMS = semver

semver_SOURCES = semver.cpp

# Set include path
semver_CPPFLAGS  = -I$(top_srcdir)/src
semver_CPPFLAGS += -DLOCALEDIR=\"$(localedir)\"

# Link semver against dependent libraries
semver_LDADD  = $(top_builddir)/src/libsemver/libsemver.la

# Link semver against libintl if gettext is being used
semver_LDADD += @LTLIBINTL@
