cmake_minimum_required(VERSION 3.8)
project(semver-utils)

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

set(LIBSEMVER_SOURCE_FILES
        src/libsemver/c/errors.h
        src/libsemver/c/libsemver.cpp
        src/libsemver/c/libsemver.h
        src/libsemver/gettext.h
        src/libsemver/gettext_defs.h
        src/libsemver/c++/version.cpp
        src/libsemver/c++/version.hpp)
add_library(libsemver ${LIBSEMVER_SOURCE_FILES})
target_include_directories(libsemver PUBLIC src)

set(SEMVER_SOURCE_FILES
        src/semver/semver.cpp
        libsemver_config.h)

add_definitions(-DLOCALEDIR)

add_executable(semver-utils ${SEMVER_SOURCE_FILES})
target_link_libraries (semver-utils libsemver)