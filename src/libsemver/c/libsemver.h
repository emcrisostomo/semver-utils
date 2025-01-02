/*
 * Copyright (c) 2016-2024 Enrico M. Crisostomo
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
 * @brief Header of the `libsemver` library.
 *
 * This header file defines the API of the `libsemver` library.
 *
 * @copyright Copyright (c) 2016-2024 Enrico M. Crisostomo
 * @license GNU General Public License v. 3.0
 * @author Enrico M. Crisostomo
 * @version 3.0.0
 */

#ifndef LIBSEMVER_H
#define LIBSEMVER_H

#include <stdbool.h>

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

#  ifdef __cplusplus
extern "C"
{
#  endif

typedef struct semver_t
{
  void *ptr;
} semver_t;

int semver_last_error();

semver_t *semver_from_string(const char *str);
semver_t *semver_create(const unsigned int *c_vers,
                        const unsigned long c_vers_num,
                        const char *prerelease,
                        const char *metadata);
void semver_free(semver_t *ver);

const char *semver_str(semver_t *ver);
const char * const *semver_get_versions(semver_t *ver);
const char *semver_get_version(semver_t *ver, unsigned int index);
const char *semver_get_prerelease(semver_t *ver);
const char *semver_get_metadata(semver_t *ver);
semver_t *semver_bump_major(semver_t *ver);
semver_t *semver_bump_minor(semver_t *ver);
semver_t *semver_bump_patch(semver_t *ver);
semver_t *semver_bump(semver_t *ver, unsigned int index);
semver_t *semver_strip_prerelease(semver_t *ver);
semver_t *semver_strip_metadata(semver_t *ver);
bool semver_is_release(semver_t *ver);
bool semver_equals(semver_t *lh, semver_t *rh);
bool semver_is_less(semver_t *lh, semver_t *rh);
bool semver_is_greater(semver_t *lh, semver_t *rh);

#  ifdef __cplusplus
}
#  endif

#endif /* LIBSEMVER_H */

#pragma clang diagnostic pop