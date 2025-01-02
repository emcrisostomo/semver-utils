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
 * @mainpage
 *
 * @section introduction Introduction
 *
 * `libsemver` is a C++ library with C bindings that provides the following
 * functionality:
 *
 *   - Parsing a version number into an object.
 *   - Comparing versions.
 *   - Modifying versions object.
 *
 * Versions are checked against _Semantic Versioning 2.0.0_
 * (http://semver.org/).
 *
 * @section changelog Changelog
 *
 * See the @ref history "History" page.
 *
 * @section bindings Available Bindings
 *
 * `libsemver` is a C++ library with C bindings which makes it available to a
 * wide range of programming languages.  If a programming language has C
 * bindings, then `libsemver` can be used from it.  The C binding provides all
 * the functionality provided by the C++ implementation and it can be used as a
 * fallback solution when the C++ API cannot be used.
 *
 * @section libtools-versioning libtool's versioning scheme
 *
 * `libtool`'s versioning scheme is described by three integers:
 * `current:revision:age` where:
 *
 *   - `current` is the most recent interface number implemented by the
 *     library.
 *   - `revision` is the implementation number of the current interface.
 *   - `age` is the difference between the newest and the oldest interface that
 *     the library implements.
 *
 * @section bug-reports Reporting Bugs and Suggestions
 *
 * If you find problems or have suggestions about this program or this manual,
 * please report them as new issues in the official GitHub repository at
 * https://github.com/emcrisostomo/semver-utils.  Please, read the
 * `CONTRIBUTING.md` file for detailed instructions on how to contribute to
 * `fswatch`.

 */
/**
 * @page history History
 *
 * @section libv100 1:0:0
 *
 *   - Initial release.
 * 
 * @section libv200 2:0:0
 * 
 *   - Added support for C bindings.
 *   - Added support for CMake build system.
 * 
 * @section libv300 3:0:0
 * 
 *   - Changed interface to represent version components as std::string instead
 *     of unsigned int.
 *   - Added tests to the C++ API using the semver binary with full coverage
 *     of the cases specified in the Semantic Versioning 2.0.0 specification at
 *     http://semver.org/.
 */
#include <algorithm>
#include <iterator> // back_inserter
#include <regex>
#include <iostream>
#include "../gettext_defs.h"
#include "version.hpp"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

namespace semver
{
  static void match_prerelease(const std::string& s);
  static void match_metadata(const std::string& s);
  static void check_identifier(const std::string& s);
  static const std::string PRERELEASE_PATTERN(
    "([0-9A-Za-z-]+(\\.[0-9A-Za-z-]+)*)");
  static const std::string METADATA_PATTERN(
    "([0-9A-Za-z-]+(\\.[0-9A-Za-z-]+)*)");

  prerelease_component::prerelease_component(std::string s) :
    identifier(std::move(s))
  {
    is_number = !identifier.empty()
                && std::all_of(identifier.begin(), identifier.end(), ::isdigit);
  }

  bool prerelease_component::operator==(const prerelease_component& v) const
  {
    return identifier == v.identifier;
  }

  bool prerelease_component::operator<(const prerelease_component& rh) const
  {
    if (is_number && rh.is_number)
    {
      // Pad the numeric strings with leading zeros to ensure they have the same length
      size_t max_length = std::max(identifier.size(), rh.identifier.size());
      std::string this_padded = std::string(max_length - identifier.size(), '0') + identifier;
      std::string rh_padded = std::string(max_length - rh.identifier.size(), '0') + rh.identifier;
      return this_padded < rh_padded;
    }
    if (!is_number && !rh.is_number) return identifier < rh.identifier;
    return is_number;
  }

  bool prerelease_component::operator>(const prerelease_component& rh) const
  {
    return (rh < *this);
  }

  version version::from_string(std::string v)
  {
    // Moving the input parameter in order not to change the API interface
    // and be efficient.  Be careful _not_ to use the input parameter after
    // this point.
    const std::string version = std::move(v);
    // Using the ECMAScript regular expression grammar to match the version
    // as specified in https://semver.org/#is-there-a-suggested-regular-expression-regex-to-check-a-semver-string
    const std::string semver_ecma = "^(0|[1-9]\\d*)\\.(0|[1-9]\\d*)\\.(0|[1-9]\\d*)(?:-((?:0|[1-9]\\d*|\\d*[a-zA-Z-][0-9a-zA-Z-]*)(?:\\.(?:0|[1-9]\\d*|\\d*[a-zA-Z-][0-9a-zA-Z-]*))*))?(?:\\+([0-9a-zA-Z-]+(?:\\.[0-9a-zA-Z-]+)*))?$";
    std::regex semver_ecma_regexp(semver_ecma, std::regex_constants::ECMAScript);
    std::smatch ecma_fragments;
    if (!std::regex_match(version, ecma_fragments, semver_ecma_regexp))
      throw std::invalid_argument(_("Invalid version: ") + version);

    return semver::version(
            ecma_fragments[1],
            ecma_fragments[2],
            ecma_fragments[3],
            ecma_fragments[4],
            ecma_fragments[5]);
  }

  version::version(std::vector<std::string> versions,
                   std::string prerelease,
                   std::string metadata) :
    versions(std::move(versions)),
    prerelease(std::move(prerelease)),
    metadata(std::move(metadata))
  {
    if (this->versions.size() < 2)
      throw std::invalid_argument("Version must contain at least two numbers.");

    if (!this->prerelease.empty())
    {
      match_prerelease(this->prerelease);
      parse_prerelease();
    }

    if (!this->metadata.empty())
    {
      match_metadata(this->metadata);
    }
  }

  version::version(std::string major,
                   std::string minor,
                   std::string patch,
                   std::string prerelease,
                   std::string metadata) :
    versions{std::move(major), std::move(minor), std::move(patch)},
    prerelease(std::move(prerelease)),
    metadata(std::move(metadata))
  {
  }

  std::string version::str() const
  {
    std::string out = versions[0];

    for (size_t i = 1; i < versions.size(); ++i)
    {
      out += ".";
      out += versions[i];
    }

    if (!prerelease.empty())
    {
      out += "-";
      out += prerelease;
    }

    if (!metadata.empty())
    {
      out += "+";
      out += metadata;
    }

    return out;
  }

  version version::bump_major() const
  {
    return bump(0);
  }

  version version::bump_minor() const
  {
    return bump(1);
  }

  version version::bump_patch() const
  {
    return bump(2);
  }

  static std::string increment_version_component(const std::string& version_component) {
    std::string result = version_component;
    int carry = 1; // Start with the increment

    for (int i = result.size() - 1; i >= 0 && carry; --i) {
        if (isdigit(result[i])) {
            int digit = result[i] - '0' + carry;
            carry = digit / 10;
            result[i] = '0' + (digit % 10);
        } else {
            throw std::invalid_argument("Version component contains non-digit characters");
        }
    }

    if (carry) {
        result.insert(result.begin(), '1');
    }

    return result;
}

  version version::bump(unsigned int index) const
  {
    std::vector<std::string> bumped_versions = versions;

    if (index >= bumped_versions.size())
    {
      std::fill_n(std::back_inserter(bumped_versions),
                  index - bumped_versions.size() + 1,
                  std::string("0"));
      bumped_versions[index] = std::string("1");
    }
    else
    {
      bumped_versions[index] = increment_version_component(bumped_versions[index]);

      for (size_t i = index + 1; i < bumped_versions.size(); ++i)
        bumped_versions[i] = "0";
    }

    return version(bumped_versions, prerelease, metadata);
  }

  const std::vector<std::string>& version::get_version() const
  {
    return versions;
  }

  std::string version::get_prerelease() const
  {
    return prerelease;
  }

  std::string version::get_metadata() const
  {
    return metadata;
  }

  version version::strip_prerelease() const
  {
    return version(versions, "", metadata);
  }

  version version::strip_metadata() const
  {
    return version(versions, prerelease, "");
  }

  bool version::is_release() const
  {
    return (prerelease.empty());
  }

  // Helper function to pad strings with leading zeros
  std::string pad_string(const std::string& str, size_t length) {
    if (str.size() < length) {
        return std::string(length - str.size(), '0') + str;
    }
    return str;
  }

  bool version::operator==(const version& v) const
  {
    return versions == v.versions && prerelease == v.prerelease;
  }

  bool version::operator<(const version& v) const
  {
    // Determine the maximum length of version components for padding
    size_t max_length = 0;
    for (const auto& s : versions) {
      max_length = std::max(max_length, s.size());
    }
    for (const auto& s : v.versions) {
      max_length = std::max(max_length, s.size());
    }

    // Compare version numbers as padded strings
    for (size_t i = 0; i < std::min(versions.size(), v.versions.size()); ++i) {
      std::string this_version_padded = pad_string(versions[i], max_length);
      std::string other_version_padded = pad_string(v.versions[i], max_length);

      if (this_version_padded < other_version_padded) return true;
      if (this_version_padded > other_version_padded) return false;
    }

    // If all compared components are equal, the version with fewer components is smaller.
    if (versions.size() < v.versions.size()) return true;
    if (versions.size() > v.versions.size()) return false;


    // Compare prerelease identifiers.
    if (prerelease == v.prerelease) return false;

    // If either one, but not both, are release versions, release is greater.
    if (is_release() ^ v.is_release()) return !is_release();

    return prerelease_comp < v.prerelease_comp;
  }

  bool version::operator>(const version& v) const
  {
    return (v < *this);
  }

  std::string version::get_version(unsigned int index) const
  {
    if (index >= versions.size()) return std::string("0");

    return versions[index];
  }

  void version::parse_prerelease()
  {
    std::regex separator("\\.");
    std::sregex_token_iterator first(prerelease.begin(), prerelease.end(), separator, -1);
    std::sregex_token_iterator last;

    std::for_each(
      first,
      last,
      [this](std::string s)
      {
        check_identifier(s);

        prerelease_comp.emplace_back(s);
      });
  }

  static void check_identifier(const std::string& s)
  {
    if (s.empty())
      throw std::invalid_argument(_("Invalid identifier: ") + s);

    if (s[0] != '0') return;

    for (size_t i = 1; i < s.size(); ++i)
    {
      if (!std::isdigit(s[i])) return;
    }

    throw std::invalid_argument(
      _("Numerical identifier cannot contain leading zeroes."));
  }

  static void match_prerelease(const std::string& s)
  {
    if (!std::regex_match(s, std::regex(PRERELEASE_PATTERN)))
      throw std::invalid_argument(_("Invalid prerelease: ") + s);
  }

  static void match_metadata(const std::string& s)
  {
    if (!std::regex_match(s, std::regex(METADATA_PATTERN)))
      throw std::invalid_argument(_("Invalid metadata: ") + s);
  }
}

#pragma clang diagnostic pop
