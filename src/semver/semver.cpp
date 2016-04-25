#include <iostream>
#include "libsemver/c++/version.hpp"

int main(int argc, char **argv)
{
  for (int i = 1; i < argc; ++i)
  {
    try
    {
      semver::version v = semver::version::from_string(argv[i]);
      std::cout << "Successully built version: " << argv[i] << "\n";
      std::cout << "Version: " << v.str() << "\n";
    }
    catch (std::invalid_argument& ex)
    {
      std::cerr << ex.what() << "\n";
    }
  }
}
