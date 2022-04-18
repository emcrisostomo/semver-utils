/*
 * Copyright (c) 2016 Enrico M. Crisostomo
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 3, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */
/**
 * @file
 * @brief Main `libsemver` source file.
 * @copyright Copyright (c) 2016 Enrico M. Crisostomo
 * @license GNU General Public License v. 3.0
 * @author Enrico M. Crisostomo
 * @version 1.0.0
 */

#include "libsemver.h"
#include "errors.h"
#include "../c++/version.hpp"
#include "../gettext_defs.h"
#include <exception>
#include <cstdlib>
#include <memory>
#include <stdexcept>

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

#ifdef HAVE_CXX_THREAD_LOCAL
  #define THREAD_LOCAL thread_local
#else
  #define THREAD_LOCAL
#endif

static THREAD_LOCAL int last_error;

static void semver_set_last_error(int err);
static void semver_reset_last_error();

int semver_last_error()
{
  return last_error;
}

void semver_set_last_error(int err)
{
  last_error = err;
}

void semver_reset_last_error()
{
  last_error = SEMVER_EXIT_OK;
}

semver_t *semver_from_string(const char *str)
{
  semver_reset_last_error();

  try
  {
    std::unique_ptr<semver::version> version(
      new semver::version(semver::version::from_string(str)));
    semver_t *sv = new semver_t();
    sv->ptr = version.release();

    return sv;
  }
  catch (std::invalid_argument& ex)
  {
    semver_set_last_error(SEMVER_EXIT_INVALID_VERSION);
    return nullptr;
  }
  catch (std::bad_alloc& ex)
  {
    semver_set_last_error(SEMVER_EXIT_BAD_ALLOC);
    return nullptr;
  }
}

semver_t *semver_create(const unsigned int *c_vers,
                        const unsigned long c_vers_num,
                        const char *prerelease,
                        const char *metadata)
{
  semver_reset_last_error();

  try
  {
    std::unique_ptr<semver::version> version(
      new semver::version(
        std::vector<unsigned int>(c_vers, c_vers + c_vers_num),
        prerelease ? prerelease : "",
        metadata ? metadata : ""));
    semver_t *sv = new semver_t();
    sv->ptr = version.release();

    return sv;
  }
  catch (std::bad_alloc& ex)
  {
    semver_set_last_error(SEMVER_EXIT_BAD_ALLOC);
    return nullptr;
  }

}

void semver_free(semver_t *ver)
{
  delete static_cast<semver::version *>(ver->ptr);
  delete ver;
}

const char *semver_str(semver_t *ver)
{
  semver_reset_last_error();

  try
  {
    semver::version *version = static_cast<semver::version *>(ver->ptr);
    std::string str = version->str();

    char *c_str = (char *) malloc(sizeof(char) * (str.size() + 1));

    if (!c_str) throw std::bad_alloc();

    std::copy(str.begin(), str.end(), c_str);
    c_str[str.size()] = '\0';

    return c_str;
  }
  catch (std::bad_alloc& ex)
  {
    semver_set_last_error(SEMVER_EXIT_BAD_ALLOC);
    return nullptr;
  }
}

unsigned int *semver_get_versions(semver_t *ver)
{
  semver_reset_last_error();

  try
  {
    semver::version *version = static_cast<semver::version *>(ver->ptr);
    std::vector<unsigned int> versions = version->get_version();

    unsigned int *c_vers =
      (unsigned int *) malloc(sizeof(unsigned int) * versions.size());

    if (!c_vers) throw std::bad_alloc();

    std::copy(versions.begin(), versions.end(), c_vers);

    return c_vers;
  }
  catch (std::bad_alloc& ex)
  {
    semver_set_last_error(SEMVER_EXIT_BAD_ALLOC);
    return nullptr;
  }
}

unsigned int semver_get_version(semver_t *ver, unsigned int index)
{
  semver_reset_last_error();

  try
  {
    semver::version *version = static_cast<semver::version *>(ver->ptr);

    return version->get_version(index);
  }
  catch (std::bad_alloc& ex)
  {
    semver_set_last_error(SEMVER_EXIT_BAD_ALLOC);
    return 0;
  }
}

const char *semver_get_prerelease(semver_t *ver)
{
  semver_reset_last_error();

  try
  {
    semver::version *version = static_cast<semver::version *>(ver->ptr);
    std::string prerelease = version->get_prerelease();

    char *c_str = (char *) malloc(sizeof(char) * (prerelease.size() + 1));

    if (!c_str) return nullptr;

    std::copy(prerelease.begin(), prerelease.end(), c_str);
    c_str[prerelease.size()] = '\0';

    return c_str;
  }
  catch (std::bad_alloc& ex)
  {
    semver_set_last_error(SEMVER_EXIT_BAD_ALLOC);
    return nullptr;
  }
}

const char *semver_get_metadata(semver_t *ver)
{
  semver_reset_last_error();

  try
  {
    semver::version *version = static_cast<semver::version *>(ver->ptr);

    std::string metadata = version->get_metadata();

    char *ptr_s = (char *) malloc(sizeof(char) * (metadata.size() + 1));

    if (!ptr_s) return nullptr;

    std::copy(metadata.begin(), metadata.end(), ptr_s);
    ptr_s[metadata.size()] = '\0';

    return ptr_s;
  }
  catch (std::bad_alloc& ex)
  {
    semver_set_last_error(SEMVER_EXIT_BAD_ALLOC);
    return nullptr;
  }
}

semver_t *semver_bump_major(semver_t *ver)
{
  semver_reset_last_error();

  try
  {
    semver::version *version = static_cast<semver::version *>(ver->ptr);
    std::unique_ptr<semver::version> bumped_version(
      new semver::version(version->bump_major()));

    semver_t *sv = new semver_t();
    sv->ptr = bumped_version.release();

    return sv;
  }
  catch (std::bad_alloc& ex)
  {
    semver_set_last_error(SEMVER_EXIT_BAD_ALLOC);
    return nullptr;
  }
}

semver_t *semver_bump_minor(semver_t *ver)
{
  semver_reset_last_error();

  try
  {
    semver::version *version = static_cast<semver::version *>(ver->ptr);
    std::unique_ptr<semver::version> bumped_version(
      new semver::version(version->bump_minor()));

    semver_t *sv = new semver_t();
    sv->ptr = bumped_version.release();

    return sv;
  }
  catch (std::bad_alloc& ex)
  {
    semver_set_last_error(SEMVER_EXIT_BAD_ALLOC);
    return nullptr;
  }
}

semver_t *semver_bump_patch(semver_t *ver)
{
  semver_reset_last_error();

  try
  {
    semver::version *version = static_cast<semver::version *>(ver->ptr);
    std::unique_ptr<semver::version> bumped_version(
      new semver::version(version->bump_patch()));

    semver_t *sv = new semver_t();
    sv->ptr = bumped_version.release();

    return sv;
  }
  catch (std::bad_alloc& ex)
  {
    semver_set_last_error(SEMVER_EXIT_BAD_ALLOC);
    return nullptr;
  }
}

semver_t *semver_bump(semver_t *ver, unsigned int index)
{
  semver_reset_last_error();

  try
  {
    semver::version *version = static_cast<semver::version *>(ver->ptr);
    std::unique_ptr<semver::version> bumped_version(
      new semver::version(version->bump(index)));

    semver_t *sv = new semver_t();
    sv->ptr = bumped_version.release();

    return sv;
  }
  catch (std::bad_alloc& ex)
  {
    semver_set_last_error(SEMVER_EXIT_BAD_ALLOC);
    return nullptr;
  }
}

semver_t *semver_strip_prerelease(semver_t *ver)
{
  semver_reset_last_error();

  try
  {
    semver::version *version = static_cast<semver::version *>(ver->ptr);
    std::unique_ptr<semver::version> bumped_version(
      new semver::version(version->strip_prerelease()));

    semver_t *sv = new semver_t();
    sv->ptr = bumped_version.release();

    return sv;
  }
  catch (std::bad_alloc& ex)
  {
    semver_set_last_error(SEMVER_EXIT_BAD_ALLOC);
    return nullptr;
  }
}

semver_t *semver_strip_metadata(semver_t *ver)
{
  semver_reset_last_error();

  try
  {
    semver::version *version = static_cast<semver::version *>(ver->ptr);
    std::unique_ptr<semver::version> bumped_version(
      new semver::version(version->strip_metadata()));

    semver_t *sv = new semver_t();
    sv->ptr = bumped_version.release();

    return sv;
  }
  catch (std::bad_alloc& ex)
  {
    semver_set_last_error(SEMVER_EXIT_BAD_ALLOC);
    return nullptr;
  }
}

bool semver_is_release(semver_t *ver)
{
  semver_reset_last_error();

  semver::version *version = static_cast<semver::version *>(ver->ptr);

  return version->is_release();
}

bool semver_equals(semver_t *lh, semver_t *rh)
{
  semver_reset_last_error();

  semver::version *lhs = static_cast<semver::version *>(lh->ptr);
  semver::version *rhs = static_cast<semver::version *>(rh->ptr);

  return *lhs == *rhs;
}

bool semver_is_less(semver_t *lh, semver_t *rh)
{
  semver_reset_last_error();

  semver::version *lhs = static_cast<semver::version *>(lh->ptr);
  semver::version *rhs = static_cast<semver::version *>(rh->ptr);

  return *lhs < *rhs;
}

bool semver_is_greater(semver_t *lh, semver_t *rh)
{
  semver_reset_last_error();

  semver::version *lhs = static_cast<semver::version *>(lh->ptr);
  semver::version *rhs = static_cast<semver::version *>(rh->ptr);

  return *lhs > *rhs;
}

#pragma clang diagnostic pop
