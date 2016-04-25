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

  semver::version good({1, 0, 0}, "abc.1.a", "001");
  std::cout << "Good: " << good.str() << "\n";

  try
  {
    semver::version bad({1, 0, 0}, "abc.01.a", "001");
  }
  catch (std::invalid_argument& ex)
  {
    std::cerr << ex.what() << "\n";
  }
}
