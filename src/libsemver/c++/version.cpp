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
  static std::vector<unsigned int> parse_version(std::string v);
  static void match_prerelease(const std::string& s);
  static void match_metadata(const std::string& s);
  static void check_prerelease(const std::string& s);
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

    std::vector<unsigned int> version =
      parse_version(fragments[VERSION_INDEX].str());
    std::string prerelease = fragments[PRERELEASE_INDEX].str();
    std::string metadata = fragments[METADATA_INDEX].str();

    return semver::version(version, prerelease, metadata);
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
      check_prerelease(prerelease);
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

  version version::release() const
  {
    return version(versions, "", metadata);
  }

  bool version::is_release() const
  {
    return (prerelease.size() == 0);
  }

  std::vector<unsigned int> parse_version(std::string v)
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

  void check_prerelease(const std::string& s)
  {
    std::regex separator("\\.");
    std::sregex_token_iterator first(s.begin(), s.end(), separator, -1);
    std::sregex_token_iterator last;

    std::for_each(
      first,
      last,
      [](std::string t)
      {
        check_identifier(t);
      }
    );
  }

  void check_identifier(const std::string& s)
  {
    if (s.size() == 0)
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