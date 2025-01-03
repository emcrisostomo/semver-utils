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
project(semver-utils VERSION 2.0.0 LANGUAGES C CXX)

#@formatter:off
set(PACKAGE           "semver-utils")
set(PACKAGE_NAME      "${PACKAGE}")
set(PACKAGE_VERSION   "${PROJECT_VERSION}")
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

set(SEMVER_EXECUTABLE ./semver)

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

add_test(versionFails0 semver -v 1)
set_tests_properties(versionFails0 PROPERTIES WILL_FAIL true)
add_test(versionFails1 semver -v 1.2)
set_tests_properties(versionFails1 PROPERTIES WILL_FAIL true)
add_test(versionFails2 semver -v 1.2.3-0123)
set_tests_properties(versionFails2 PROPERTIES WILL_FAIL true)
add_test(versionFails3 semver -v 1.2.3-0123.0123)
set_tests_properties(versionFails3 PROPERTIES WILL_FAIL true)
add_test(versionFails4 semver -v 1.1.2+.123)
set_tests_properties(versionFails4 PROPERTIES WILL_FAIL true)
add_test(versionFails5 semver -v +invalid)
set_tests_properties(versionFails5 PROPERTIES WILL_FAIL true)
add_test(versionFails6 semver -v -invalid)
set_tests_properties(versionFails6 PROPERTIES WILL_FAIL true)
add_test(versionFails7 semver -v -invalid+invalid)
set_tests_properties(versionFails7 PROPERTIES WILL_FAIL true)
add_test(versionFails8 semver -v -invalid.01)
set_tests_properties(versionFails8 PROPERTIES WILL_FAIL true)
add_test(versionFails9 semver -v alpha)
set_tests_properties(versionFails9 PROPERTIES WILL_FAIL true)
add_test(versionFails10 semver -v alpha.beta)
set_tests_properties(versionFails10 PROPERTIES WILL_FAIL true)
add_test(versionFails11 semver -v alpha.beta.1)
set_tests_properties(versionFails11 PROPERTIES WILL_FAIL true)
add_test(versionFails12 semver -v alpha.1)
set_tests_properties(versionFails12 PROPERTIES WILL_FAIL true)
add_test(versionFails13 semver -v alpha+beta)
set_tests_properties(versionFails13 PROPERTIES WILL_FAIL true)
add_test(versionFails14 semver -v alpha_beta)
set_tests_properties(versionFails14 PROPERTIES WILL_FAIL true)
add_test(versionFails15 semver -v alpha.)
set_tests_properties(versionFails15 PROPERTIES WILL_FAIL true)
add_test(versionFails16 semver -v alpha..)
set_tests_properties(versionFails16 PROPERTIES WILL_FAIL true)
add_test(versionFails17 semver -v beta)
set_tests_properties(versionFails17 PROPERTIES WILL_FAIL true)
add_test(versionFails18 semver -v 1.0.0-alpha_beta)
set_tests_properties(versionFails18 PROPERTIES WILL_FAIL true)
add_test(versionFails19 semver -v -alpha.)
set_tests_properties(versionFails19 PROPERTIES WILL_FAIL true)
add_test(versionFails20 semver -v 1.0.0-alpha..)
set_tests_properties(versionFails20 PROPERTIES WILL_FAIL true)
add_test(versionFails21 semver -v 1.0.0-alpha..1)
set_tests_properties(versionFails21 PROPERTIES WILL_FAIL true)
add_test(versionFails22 semver -v 1.0.0-alpha...1)
set_tests_properties(versionFails22 PROPERTIES WILL_FAIL true)
add_test(versionFails23 semver -v 1.0.0-alpha....1)
set_tests_properties(versionFails23 PROPERTIES WILL_FAIL true)
add_test(versionFails24 semver -v 1.0.0-alpha.....1)
set_tests_properties(versionFails24 PROPERTIES WILL_FAIL true)
add_test(versionFails25 semver -v 1.0.0-alpha......1)
set_tests_properties(versionFails25 PROPERTIES WILL_FAIL true)
add_test(versionFails26 semver -v 1.0.0-alpha.......1)
set_tests_properties(versionFails26 PROPERTIES WILL_FAIL true)
add_test(versionFails27 semver -v 01.1.1)
set_tests_properties(versionFails27 PROPERTIES WILL_FAIL true)
add_test(versionFails28 semver -v 1.01.1)
set_tests_properties(versionFails28 PROPERTIES WILL_FAIL true)
add_test(versionFails29 semver -v 1.1.01)
set_tests_properties(versionFails29 PROPERTIES WILL_FAIL true)
add_test(versionFails30 semver -v 1.2)
set_tests_properties(versionFails30 PROPERTIES WILL_FAIL true)
add_test(versionFails31 semver -v 1.2.3.DEV)
set_tests_properties(versionFails31 PROPERTIES WILL_FAIL true)
add_test(versionFails32 semver -v 1.2-SNAPSHOT)
set_tests_properties(versionFails32 PROPERTIES WILL_FAIL true)
add_test(versionFails33 semver -v 1.2.31.2.3----RC-SNAPSHOT.12.09.1--..12+788)
set_tests_properties(versionFails33 PROPERTIES WILL_FAIL true)
add_test(versionFails34 semver -v 1.2-RC-SNAPSHOT)
set_tests_properties(versionFails34 PROPERTIES WILL_FAIL true)
add_test(versionFails35 semver -v -1.0.3-gamma+b7718)
set_tests_properties(versionFails35 PROPERTIES WILL_FAIL true)
add_test(versionFails36 semver -v +justmeta)
set_tests_properties(versionFails36 PROPERTIES WILL_FAIL true)
add_test(versionFails37 semver -v 9.8.7+meta+meta)
set_tests_properties(versionFails37 PROPERTIES WILL_FAIL true)
add_test(versionFails38 semver -v 9.8.7-whatever+meta+meta)
set_tests_properties(versionFails38 PROPERTIES WILL_FAIL true)
add_test(versionFails39 semver -v 99999999999999999999999.999999999999999999.99999999999999999----RC-SNAPSHOT.12.09.1--------------------------------..12)
set_tests_properties(versionFails39 PROPERTIES WILL_FAIL true)

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

# Testing comparison: including all tests from https://semver.org
add_test(semverCompare ${CMAKE_SOURCE_DIR}/check_exit_code.sh 1 ${SEMVER_EXECUTABLE} -c 1.2.3 7.8.9)
add_test(semverCompare2 ${CMAKE_SOURCE_DIR}/check_exit_code.sh 2 ${SEMVER_EXECUTABLE} -c 7.8.9 1.2.3)
add_test(semverCompare3 ${CMAKE_SOURCE_DIR}/check_exit_code.sh 2 ${SEMVER_EXECUTABLE} -c 71.8.9 1.2.3)
add_test(semverCompare4 ${CMAKE_SOURCE_DIR}/check_exit_code.sh 2 ${SEMVER_EXECUTABLE} -c 17.8.9 2.2.3)
add_test(semverCompare5 ${CMAKE_SOURCE_DIR}/check_exit_code.sh 2 ${SEMVER_EXECUTABLE} -c 1.17.3 1.2.3)
add_test(semverCompare6 ${CMAKE_SOURCE_DIR}/check_exit_code.sh 1 ${SEMVER_EXECUTABLE} -c 1.0.0-alpha 1.0.0)
add_test(semverCompare7 ${CMAKE_SOURCE_DIR}/check_exit_code.sh 1 ${SEMVER_EXECUTABLE} -c 1.0.0-alpha 1.0.0-alpha.1)
add_test(semverCompare8 ${CMAKE_SOURCE_DIR}/check_exit_code.sh 1 ${SEMVER_EXECUTABLE} -c 1.0.0-alpha.1 1.0.0-alpha.beta)
add_test(semverCompare9 ${CMAKE_SOURCE_DIR}/check_exit_code.sh 1 ${SEMVER_EXECUTABLE} -c 1.0.0-alpha.beta 1.0.0-beta)
add_test(semverCompare10 ${CMAKE_SOURCE_DIR}/check_exit_code.sh 1 ${SEMVER_EXECUTABLE} -c 1.0.0-beta 1.0.0-beta.2)
add_test(semverCompare11 ${CMAKE_SOURCE_DIR}/check_exit_code.sh 1 ${SEMVER_EXECUTABLE} -c 1.0.0-beta.2 1.0.0-beta.11)
add_test(semverCompare12 ${CMAKE_SOURCE_DIR}/check_exit_code.sh 1 ${SEMVER_EXECUTABLE} -c 1.0.0-beta.11 1.0.0-rc.1)
add_test(semverCompare13 ${CMAKE_SOURCE_DIR}/check_exit_code.sh 1 ${SEMVER_EXECUTABLE} -c 1.0.0-rc.1 1.0.0)

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