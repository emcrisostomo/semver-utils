#                                               -*- Autoconf -*-
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
# Process this file with autoconf to produce a configure script.
#
dnl Copyright (C) 2016 Enrico M. Crisostomo
AC_PREREQ([2.69])
m4_include([m4/libsemver_version.m4])
AC_INIT([libsemver], [LIBSEMVER_VERSION], [enrico.m.crisostomo@gmail.com], [], [https://github.com/emcrisostomo/semver-utils])
AC_COPYRIGHT([2016 (C) Enrico M. Crisostomo])
AC_REVISION([$Revision: LIBSEMVER_REVISION$])
AC_CONFIG_SRCDIR([src/libsemver/c/libsemver.cpp])
AC_CONFIG_AUX_DIR([config])
AC_CONFIG_HEADERS([libsemver_config.h])
AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([src/libsemver/Makefile])
AC_CONFIG_FILES([src/semver/Makefile])
AC_CONFIG_FILES([po/Makefile.in])
AC_CONFIG_FILES([doc/doxygen/Makefile])
AC_CONFIG_MACRO_DIRS([m4])
AC_CONFIG_MACRO_DIR([m4])

# Initialize Automake
AM_INIT_AUTOMAKE([-Wall -Werror gnu subdir-objects 1.14.1])
AM_SILENT_RULES([yes])
AM_PROG_AR

# Initialize libtool
LT_PREREQ([2.4.2])
LT_INIT
AC_SUBST([LIBTOOL_DEPS])

# Initialize gettext
AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_VERSION([0.19.4])

# Configure language
AC_LANG(C++)

# Checks for programs.
AC_PROG_CXX

AC_CHECK_PROGS([DOXYGEN], [doxygen])
test -z "${DOXYGEN}" && AC_MSG_WARN([Doxygen support disabled])

AM_CONDITIONAL([HAVE_DOXYGEN], [test -n "${DOXYGEN}"])
AM_COND_IF([HAVE_DOXYGEN], [AC_CONFIG_FILES([doc/doxygen/Doxyfile])])

# Configure C++ compiler
AX_CXX_COMPILE_STDCXX_11
AX_CXXFLAGS_WARN_ALL

# Set library interface version
AC_SUBST([AM_LIBSEMVER_API_VERSION], LIBSEMVER_API_VERSION)

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([getopt.h])

AS_VAR_IF([ac_cv_header_getopt_h], ["yes"], [AC_CHECK_FUNCS([getopt_long])])
AS_VAR_IF([ac_cv_func_getopt_long], ["yes"], [], [AC_MSG_ERROR([Cannot found getopt_long.])])

AC_CHECK_HEADERS([unistd.h stdbool.h], [], [AC_MSG_ERROR([Cannot find a required header.])])

# Checks for typedefs, structures, and compiler characteristics.
AX_CXX_HAVE_THREAD_LOCAL

AS_VAR_IF(
  [ax_cv_cxx_have_thread_local],
  ["yes"],
  [],
  [AC_MSG_WARN([thread_local storage specified not supported: the C API of AC_PACKAGE_NAME is not thread-safe.])])

# Checks for library functions.

AC_OUTPUT
