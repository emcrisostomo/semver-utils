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
#ifndef SEMVER_UTILS_VERSION_H
#define SEMVER_UTILS_VERSION_H

#include <vector>
#include <string>

namespace semver
{
  class version
  {
  private:
    std::vector<unsigned int> versions;
    std::string prerelease;
    std::string metadata;

  public:
    static version from_string(std::string v);

    version(const std::vector<unsigned int> versions,
            const std::string prerelease = "",
            const std::string metadata = "");
    std::string str() const;
    std::vector<unsigned int> get_version() const;
    std::string get_prerelease() const;
    std::string get_metadata() const;
    version bump_major() const;
    version bump_minor() const;
    version bump_patch() const;
    version bump(unsigned int index) const;
    version release();
    bool is_release() const;
  };
}

#endif // SEMVER_UTILS_VERSION_H
