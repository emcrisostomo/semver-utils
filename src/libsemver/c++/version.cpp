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

#include <regex>
#include "../gettext_defs.h"
#include "version.hpp"

semver::version semver::version::from_string(std::string v)
{
  std::regex version_grammar("^([[:digit:]]+)(\\.[[:digit:]]+){2}(-[0-9A-Za-z-]*)?(\\+[0-9A-Za-z-]*)?$", std::regex_constants::extended);
  std::smatch fragments;

  if (!std::regex_match(v, fragments, version_grammar)) throw std::invalid_argument(_("Invalid version: ") + v);

  return semver::version({1,2,3});
}

semver::version::version(const std::vector<unsigned int> versions,
                         const std::string prerelease,
                         const std::string metadata) : versions(std::move(versions)),
                                                       prerelease(std::move(prerelease)),
                                                       metadata(std::move(metadata))
{
}