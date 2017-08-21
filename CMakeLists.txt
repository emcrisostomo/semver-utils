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
target_include_directories(libsemver PUBLIC src)

set(SEMVER_SOURCE_FILES
        src/semver/semver.cpp)

add_executable(semver ${SEMVER_SOURCE_FILES})
target_link_libraries(semver libsemver)

# Add option to choose between shared and static libraries
option(BUILD_SHARED_LIBS "Build shared libraries" ON)

# include modules
include(FindGettext)
include(FindIntl)
include(CheckIncludeFileCXX)
include(CheckSymbolExists)

include_directories("${PROJECT_BINARY_DIR}")
include_directories("${Intl_INCLUDE_DIRS}")

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