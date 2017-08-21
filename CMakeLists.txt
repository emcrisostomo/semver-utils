cmake_minimum_required(VERSION 3.8)
project(semver-utils)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
        src/libsemver/c/errors.h
        src/libsemver/c/libsemver.cpp
        src/libsemver/c/libsemver.h
        src/libsemver/gettext.h
        src/libsemver/gettext_defs.h
        src/semver/semver.cpp
        libsemver_config.h
        src/libsemver/c++/version.cpp
        src/libsemver/c++/version.hpp)

include_directories(. src)
add_definitions(-DLOCALEDIR)

add_executable(semver-utils ${SOURCE_FILES})