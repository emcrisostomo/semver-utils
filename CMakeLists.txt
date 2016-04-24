cmake_minimum_required(VERSION 3.5)
project(semver_utils)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    src/libsemver/c/libsemver.cpp
    src/libsemver/c/libsemver.h
    src/libsemver/gettext.h
    src/libsemver/gettext_defs.h
    src/semver/semver.cpp
    libsemver_config.h)

include_directories(src)
add_executable(semver_utils ${SOURCE_FILES})