#
# Copyright (c) 2016-2024 Enrico M. Crisostomo
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
cmake_minimum_required(VERSION 3.14)
project(semver-utils VERSION 1.2.0 LANGUAGES C CXX)

#@formatter:off
set(PACKAGE           "semver-utils")
set(PACKAGE_NAME      "${PACKAGE}")
set(PACKAGE_VERSION   "${PROJECT_VERSION}-develop")
set(PACKAGE_STRING    "${PACKAGE_NAME} ${PACKAGE_VERSION}")
set(PACKAGE_AUTHOR    "enrico.m.crisostomo@gmail.com")
set(PACKAGE_BUGREPORT "${PACKAGE_AUTHOR}")
set(PACKAGE_TARNAME   "${PACKAGE}")
set(PACKAGE_URL       "https://github.com/emcrisostomo/${PACKAGE}")
set(LOCALEDIR         "${CMAKE_INSTALL_PREFIX}/share/locale" CACHE FILEPATH "locale dir")
#@formatter:on

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(LIBSEMVER_HEADER_FILES
        src/libsemver/gettext.h
        src/libsemver/gettext_defs.h
        src/libsemver/c/errors.h
        src/libsemver/c/libsemver.h
        src/libsemver/c++/version.hpp)

set(LIBSEMVER_SOURCE_FILES
        ${LIBSEMVER_HEADER_FILES}
        src/libsemver/c/libsemver.cpp
        src/libsemver/c++/version.cpp)

add_library(libsemver ${LIBSEMVER_SOURCE_FILES})
set_target_properties(libsemver PROPERTIES OUTPUT_NAME semver)
target_include_directories(libsemver
        PUBLIC
        src
        PRIVATE
        ${PROJECT_BINARY_DIR})

set(SEMVER_SOURCE_FILES
        src/semver/semver.cpp)

add_executable(semver ${SEMVER_SOURCE_FILES})
target_include_directories(semver PRIVATE ${PROJECT_BINARY_DIR})
target_link_libraries(semver libsemver)

# Add option to choose between shared and static libraries
option(BUILD_SHARED_LIBS "Build shared libraries" ON)

# include modules
include(FindGettext)
include(FindIntl)
include(CheckIncludeFileCXX)
include(CheckSymbolExists)

target_include_directories(libsemver PUBLIC ${Intl_INCLUDE_DIRS})
target_include_directories(semver PUBLIC ${Intl_INCLUDE_DIRS})

# check for gettext and libintl
check_include_file_cxx(getopt.h HAVE_GETOPT_H)

if (HAVE_GETOPT_H)
    check_symbol_exists(getopt_long getopt.h HAVE_GETOPT_LONG)
endif (HAVE_GETOPT_H)

# If both gettext and libintl are found, define the USE_NLS variable to
# optionally disable them
if (Intl_FOUND AND GETTEXT_FOUND)
    option(USE_NLS "Use NLS" ON)
endif ()

if (USE_NLS)
    set(ENABLE_NLS 1)

    if (Intl_LIBRARIES)
        target_link_libraries(semver ${Intl_LIBRARIES})
    endif ()

    # gettext configuration
    set(LINGUAS en en@quot en@boldquot it es)

    foreach (language ${LINGUAS})
        configure_file(${PROJECT_SOURCE_DIR}/po/${language}.po ${PROJECT_BINARY_DIR}/${language}.po COPYONLY)
    endforeach ()

    gettext_process_pot_file(po/${PACKAGE}.pot ALL INSTALL_DESTINATION share/locale LANGUAGES ${LINGUAS})
endif ()

# Add a configuration file processed by cmake
configure_file(${PROJECT_SOURCE_DIR}/cmake_config.h.in ${PROJECT_BINARY_DIR}/cmake_config.h)
add_definitions(-DHAVE_CMAKE_CONFIG_H)
check_include_file_cxx(libsemver_config.h HAVE_CONFIG_H)

# install targets
install(TARGETS libsemver DESTINATION lib)
install(TARGETS semver DESTINATION bin)
install(FILES ${LIBSEMVER_HEADER_FILES} DESTINATION include/libsemver)

# tests
include(CTest)

# semver 2.0.0 tests (see https://semver.org and https://regex101.com/r/vkijKf/1/)

add_test(version0 semver -v 0.0.4)
add_test(version1 semver -v 1.2.3)
add_test(version2 semver -v 10.20.30)
add_test(version3 semver -v 1.1.2-prerelease+meta)
add_test(version4 semver -v 1.1.2+meta)
add_test(version5 semver -v 1.1.2+meta-valid)
add_test(version6 semver -v 1.0.0-alpha)
add_test(version7 semver -v 1.0.0-beta)
add_test(version8 semver -v 1.0.0-alpha.beta)
add_test(version9 semver -v 1.0.0-alpha.beta.1)
add_test(version10 semver -v 1.0.0-alpha.1)
add_test(version11 semver -v 1.0.0-alpha0.valid)
add_test(version12 semver -v 1.0.0-alpha.0valid)
add_test(version13 semver -v 1.0.0-alpha-a.b-c-somethinglong+build.1-aef.1-its-okay)
add_test(version14 semver -v 1.0.0-rc.1+build.1)
add_test(version15 semver -v 2.0.0-rc.1+build.123)
add_test(version16 semver -v 1.2.3-beta)
add_test(version17 semver -v 10.2.3-DEV-SNAPSHOT)
add_test(version18 semver -v 1.2.3-SNAPSHOT-123)
add_test(version19 semver -v 1.0.0)
add_test(version20 semver -v 2.0.0)
add_test(version21 semver -v 1.1.7)
add_test(version22 semver -v 2.0.0+build.1848)
add_test(version23 semver -v 2.0.1-alpha.1227)
add_test(version24 semver -v 1.0.0-alpha+beta)
add_test(version25 semver -v 1.2.3----RC-SNAPSHOT.12.9.1--.12+788)
add_test(version26 semver -v 1.2.3----R-S.12.9.1--.12+meta)
add_test(version27 semver -v 1.2.3----RC-SNAPSHOT.12.9.1--.12)
add_test(version28 semver -v 1.0.0+0.build.1-rc.10000aaa-kk-0.1)
add_test(version29 semver -v 99999999999999999999999.999999999999999999.99999999999999999)
add_test(version30 semver -v 1.0.0-0A.is.legal)


add_test(semverRuns semver)
set_tests_properties(semverRuns PROPERTIES WILL_FAIL true)

add_test(semverHelp semver --help)
set_tests_properties(semverHelp PROPERTIES PASS_REGULAR_EXPRESSION "${PACKAGE} ${PACKAGE_VERSION}")

add_test(semverValidateSucceeds semver -v 1.2.3 7.8.9-rc0)

add_test(semverValidateFailsVersionTooShort semver -v 1)
set_tests_properties(semverValidateFailsVersionTooShort PROPERTIES WILL_FAIL true)

add_test(semverValidateFailsVersionInvalid semver -v 1.a.0)
set_tests_properties(semverValidateFailsVersionInvalid PROPERTIES WILL_FAIL true)

add_test(semverBumpMajor semver -b major 1.0.0)
set_tests_properties(semverBumpMajor PROPERTIES PASS_REGULAR_EXPRESSION "2\\.0\\.0")

add_test(semverBumpMinor semver -b minor 1.0.0)
set_tests_properties(semverBumpMinor PROPERTIES PASS_REGULAR_EXPRESSION "1\\.1\\.0")

add_test(semverBumpPatch semver -b patch 1.0.0)
set_tests_properties(semverBumpPatch PROPERTIES PASS_REGULAR_EXPRESSION "1\\.0\\.1")

add_test(semverBumpCoordinate semver -b 10 1.2.3)
set_tests_properties(semverBumpCoordinate PROPERTIES PASS_REGULAR_EXPRESSION "1\\.2\\.3\\.0\\.0\\.0\\.0\\.0\\.0\\.0\\.1")

add_test(semverCompare semver -c 1.2.3 7.8.9)
set_tests_properties(semverCompare PROPERTIES WILL_FAIL true)

# installer
include(InstallRequiredSystemLibraries)
#@formatter:off
# Remove and default to project description when migrated to CMake 3.9
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A C/C++ library and an utility to compare and manipulate versions numbers complying with Semantic Versioning 2.0.0")
set(CPACK_PACKAGE_VERSION_MAJOR       "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR       "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH       "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_VENDOR              "${PACKAGE_AUTHOR}")
set(CPACK_RESOURCE_FILE_LICENSE       "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_GENERATOR                   "STGZ;TGZ")
#@formatter:on
include(CPack)