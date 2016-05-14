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
 * @section v600 1:0:0
 *
 *   - Initial release.
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
  static std::vector<unsigned int> parse_version(const std::string& v);
  static void match_prerelease(const std::string& s);
  static void match_metadata(const std::string& s);
  static void check_identifier(const std::string& s);
  static const std::string PRERELEASE_PATTERN(
    "([0-9A-Za-z-]+(\\.[0-9A-Za-z-]+)*)");
  static const std::string METADATA_PATTERN(
    "([0-9A-Za-z-]+(\\.[0-9A-Za-z-]+)*)");

  version version::from_string(std::string v)
  {
    // SemVer 2.0.0
    //
    // 2. A normal version number MUST take the form X.Y.Z where X, Y, and Z are
    //    non-negative integers, and MUST NOT contain leading zeroes. [...]
    //
    // This implementation allows for a version number containing more than 3
    // components.
    const std::string version_pattern = "((0|[1-9][[:digit:]]*)(\\.(0|[1-9][[:digit:]]*)){1,})";
    const int VERSION_INDEX = 1;

    // 9. A pre-release version MAY be denoted by appending a hyphen and a series
    //    of dot separated identifiers immediately following the patch version.
    //    Identifiers MUST comprise only ASCII alphanumerics and hyphen
    //    [0-9A-Za-z-].  Identifiers MUST NOT be empty.  Numeric identifiers MUST
    //    NOT include leading zeroes.
    const std::string prerelease_pattern = "(-" + PRERELEASE_PATTERN + ")?";
    const int PRERELEASE_INDEX = 6;

    // 10. Build metadata MAY be denoted by appending a plus sign and a series of
    //     dot separated identifiers immediately following the patch or
    //     pre-release version.  Identifiers MUST comprise only ASCII
    //     alphanumerics and hyphen [0-9A-Za-z-].  Identifiers MUST NOT be
    //     empty.
    const std::string metadata_pattern = "(\\+" + METADATA_PATTERN + ")?";
    const int METADATA_INDEX = 9;

    const std::string grammar =
      std::string("^")
      + version_pattern
      + prerelease_pattern
      + metadata_pattern
      + "$";

    std::regex version_grammar(grammar, std::regex_constants::extended);
    std::smatch fragments;

    if (!std::regex_match(v, fragments, version_grammar))
      throw std::invalid_argument(_("Invalid version: ") + v);

    return semver::version(parse_version(fragments[VERSION_INDEX].str()),
                           fragments[PRERELEASE_INDEX].str(),
                           fragments[METADATA_INDEX].str());
  }

  version::version(const std::vector<unsigned int> versions,
                   const std::string prerelease,
                   const std::string metadata) :
    versions(std::move(versions)),
    prerelease(std::move(prerelease)),
    metadata(std::move(metadata))
  {
    if (versions.size() < 2)
      throw std::invalid_argument("Version must contain at least two numbers.");

    if (prerelease.size() > 0)
    {
      match_prerelease(prerelease);
      parse_prerelease();
    }

    if (metadata.size() > 0)
    {
      match_metadata(metadata);
    }
  }

  std::string version::str() const
  {
    std::string out = std::to_string(versions[0]);

    for (auto i = 1; i < versions.size(); ++i)
    {
      out += ".";
      out += std::to_string(versions[i]);
    }

    if (prerelease.size() != 0)
    {
      out += "-";
      out += prerelease;
    }

    if (metadata.size() != 0)
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

  version version::bump(unsigned int index) const
  {
    std::vector<unsigned int> bumped_versions = versions;

    if (index >= bumped_versions.size())
    {
      std::fill_n(std::back_inserter(bumped_versions),
                  index - bumped_versions.size() + 1,
                  0);
      bumped_versions[index] = 1;
    }
    else
    {
      bumped_versions[index] += 1;

      for (int i = index + 1; i < bumped_versions.size(); ++i)
        bumped_versions[i] = 0;
    }

    return version(bumped_versions, prerelease, metadata);
  }

  std::vector<unsigned int> version::get_version() const
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
    return (prerelease.size() == 0);
  }

  bool version::operator==(const version& v) const
  {
    return versions == v.versions && prerelease == v.prerelease;
  }

  bool version::operator<(const version& v) const
  {
    // Compare version number.
    for (unsigned int i = 0; i < std::max(versions.size(), v.versions.size()); ++i)
    {
      // The shortest number is the lesser.
      if (i == versions.size()) return true;
      if (i == v.versions.size()) return false;

      unsigned int lh = get_version(i);
      unsigned int rh = v.get_version(i);

      if (lh < rh) return true;
      else if (lh > rh) return false;
    }

    // Compare prerelease identifiers.
    if (prerelease == v.prerelease) return false;

    // If either one, but not both, are release versions, release is greater.
    if (is_release() ^ v.is_release()) return !is_release();

    for (auto i = 0;
         i < std::max(prerelease_identifiers.size(), v.prerelease_identifiers.size());
         ++i)
    {
      // The shorted prerelease is the lesser.
      if (i == prerelease_identifiers.size()) return true;
      if (i == v.prerelease_identifiers.size()) return false;

      if (prerelease_is_identifier_number[i] && v.prerelease_is_identifier_number[i])
      {
        unsigned long lh = std::stoul(prerelease_identifiers[i]);
        unsigned long rh = std::stoul(v.prerelease_identifiers[i]);

        if (lh < rh) return true;
        if (lh > rh) return false;

        continue;
      }

      if (!prerelease_is_identifier_number[i] && !v.prerelease_is_identifier_number[i])
      {
        if (prerelease_identifiers[i] < v.prerelease_identifiers[i]) return true;
        if (prerelease_identifiers[i] > v.prerelease_identifiers[i]) return false;

        continue;
      }

      return prerelease_is_identifier_number[i];
    }

    return false;
  }

  bool version::operator>(const version& v) const
  {
    return (v < *this);
  }

  unsigned int version::get_version(unsigned int index) const
  {
    if (index >= versions.size()) return 0;

    return versions[index];
  }

  std::vector<unsigned int> parse_version(const std::string& v)
  {
    std::regex numbers("\\d+");
    std::sregex_token_iterator first(v.begin(), v.end(), numbers);
    std::sregex_token_iterator last;

    std::vector<unsigned int> results;
    std::for_each(
      first,
      last,
      [&results](std::string s)
      {
        results.push_back((unsigned int) std::stoul(s));
      });

    return results;
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

        bool is_number = !s.empty()
                         && std::find_if(s.begin(),
                                         s.end(),
                                         [](char c) { return !std::isdigit(c); }) == s.end();
        prerelease_is_identifier_number.push_back(is_number);
        prerelease_number.push_back(is_number ? std::stoul(s) : 0);
        prerelease_identifiers.push_back(s);
      });
  }

  void check_identifier(const std::string& s)
  {
    if (s.empty())
      throw std::invalid_argument(_("Invalid identifier: ") + s);

    if (s[0] != '0') return;

    for (int i = 1; i < s.size(); ++i)
    {
      if (!std::isdigit(s[i])) return;
    }

    throw std::invalid_argument(
      _("Numerical identifier cannot contain leading zeroes."));
  }

  void match_prerelease(const std::string& s)
  {
    if (!std::regex_match(s, std::regex(PRERELEASE_PATTERN)))
      throw std::invalid_argument(_("Invalid prerelease: ") + s);
  }

  void match_metadata(const std::string& s)
  {
    if (!std::regex_match(s, std::regex(METADATA_PATTERN)))
      throw std::invalid_argument(_("Invalid metadata: ") + s);
  }
}

#pragma clang diagnostic pop