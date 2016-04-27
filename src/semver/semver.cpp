#include <algorithm>
#include <iostream>
#include <getopt.h>
#include <unistd.h> // isatty()
#include <cstdio> // fileno()
#include <vector>
#include <libsemver/gettext.h>
#include <libsemver/gettext_defs.h>
#include "libsemver/c++/version.hpp"
#include "../../libsemver_config.h"

static bool command_set = false;
static bool bflag = false;
static bool cflag = false;
static bool rflag = false;
static bool sflag = false;
static bool vflag = false;
static std::string component_to_bump;

static void parse_opts(int argc, char **argv);
static std::vector<std::string> read_arguments(int argc, char **argv);
static void usage(std::ostream& stream);

static int bump_versions(std::vector<std::string> vers);
static int check_versions(std::vector<std::string> vers);
static int compare_versions(std::vector<std::string> vector);
static int sort_versions(std::vector<std::string> vector);

int main(int argc, char **argv)
{
  // Trigger gettext operations
#ifdef ENABLE_NLS
  setlocale(LC_ALL, "");
  bindtextdomain(PACKAGE, LOCALEDIR);
  textdomain(PACKAGE);
#endif

  parse_opts(argc, argv);

  if (!command_set)
  {
    std::cerr << _("No command has been specified.\n");
    exit(1);
  }

  // Extract arguments from the command line and for the pipe.
  std::vector<std::string> args = read_arguments(argc, argv);

  // validate options
  if (args.size() == 0)
  {
    std::cerr << _("Invalid number of arguments.\n");
    exit(1);
  }

  if (bflag) return bump_versions(args);
  if (cflag) return compare_versions(args);
  if (sflag) return sort_versions(args);
  if (vflag) return check_versions(args);

  return 0;
}

int bump_versions(std::vector<std::string> args)
{
  unsigned int bump = -1;

  if (component_to_bump.compare("M") == 0
      || component_to_bump.compare("major") == 0)
  {
    bump = 0;
  }
  else if (component_to_bump.compare("m") == 0
           || component_to_bump.compare("minor") == 0)
  {
    bump = 1;
  }
  else if (component_to_bump.compare("p") == 0
           || component_to_bump.compare("patch") == 0)
  {
    bump = 2;
  }
  else
  {
    try
    {
      bump = (unsigned int) std::stoul(component_to_bump);
    }
    catch (std::invalid_argument& ex)
    {
      std::cerr << _("Invalid version index: ") << component_to_bump << "\n";
      return 1;
    }
  }

  int ret = 0;

  for (auto& v : args)
  {
    try
    {
      std::cout << semver::version::from_string(v).bump(bump).str() << "\n";
    }
    catch (std::invalid_argument& ex)
    {
      std::cerr << ex.what() << "\n";
      ret = 1;
    }
  }

  return ret;
}

int compare_versions(std::vector<std::string> vector)
{
  if (vector.size() != 2)
  {
    std::cerr << _("Invalid number of arguments.\n");
    return 1;
  }

  try
  {
    semver::version first =
      semver::version::from_string(
        vector[0]).strip_prerelease().strip_metadata();
    semver::version second =
      semver::version::from_string(
        vector[1]).strip_prerelease().strip_metadata();

    if (first == second) return 0;
    return first < second ? 1 : 2;
  }
  catch (std::invalid_argument& ex)
  {
    std::cerr << ex.what() << "\n";
    return 4;
  }
}

int sort_versions(std::vector<std::string> args)
{
  unsigned int ret = 0;
  std::vector<semver::version> versions;

  for (auto& v : args)
  {
    try
    {
      versions.push_back(semver::version::from_string(v));
    }
    catch (std::invalid_argument& ex)
    {
      std::cerr << ex.what() << "\n";
      ret = 1;
    }
  }

  if (rflag)
    std::sort(versions.begin(),
              versions.end(),
              std::greater<semver::version>());
  else
    std::sort(versions.begin(), versions.end());

  for (auto& ver : versions)
  {
    std::cout << ver.str() << "\n";
  }

  return ret;
}

int check_versions(std::vector<std::string> vers)
{
  int ret = 0;

  for (auto v : vers)
  {
    try
    {
      semver::version::from_string(v);
    }
    catch (std::invalid_argument& ex)
    {
      std::cerr << ex.what() << "\n";
      ret = 1;
    }
  }

  return ret;
}

std::vector<std::string> read_arguments(int argc, char **argv)
{
  std::vector<std::string> arguments;

  for (auto i = optind; i < argc; ++i) arguments.push_back(argv[i]);

  // Read arguments from the standard input, if the program is not connected to
  // a terminal
  std::string pipe_input;
  if (!isatty(fileno(stdin)))
  {
    while (std::cin >> pipe_input)
    {
      arguments.push_back(pipe_input);
    }
  }

  return arguments;
}

void parse_opts(int argc, char **argv)
{
  int ch;
  std::string short_options = "b:chrsv";

  int option_index = 0;
  static struct option long_options[] = {
    {"bump",    required_argument, nullptr, 'b'},
    {"compare", no_argument,       nullptr, 'c'},
    {"help",    no_argument,       nullptr, 'h'},
    {"reverse", no_argument,       nullptr, 'r'},
    {"sort",    no_argument,       nullptr, 's'},
    {"valid",   no_argument,       nullptr, 'v'},
    {nullptr, 0,                   nullptr, 0}
  };

  while ((ch = getopt_long(argc,
                           argv,
                           short_options.c_str(),
                           long_options,
                           &option_index)) != -1)
  {
    switch (ch)
    {
    case 'b':
      command_set = true;
      bflag = true;
      component_to_bump = std::string(optarg);
      break;

    case 'c':
      command_set = true;
      cflag = true;
      break;

    case 'h':
      usage(std::cout);
      exit(0);

    case 'r':
      rflag = true;
      break;

    case 's':
      command_set = true;
      sflag = true;
      break;

    case 'v':
      command_set = true;
      vflag = true;
      break;

    case '?':
      exit(1);

    default:
      usage(std::cerr);
      exit(2);
    }
  }

  if (rflag && !sflag)
  {
    std::cerr << _("-r can only be used with -s.\n");
    exit(1);
  }
}

void usage(std::ostream& stream)
{
  stream << PACKAGE_STRING << "\n\n";
  stream << _("Usage:\n");
  stream << "semver" << _(" [OPTION] ... path ...\n");
  stream << "\n";
  stream << _("Options:\n");
  stream << " -h, --help            " << _("Show this message.\n");
  stream << "\n";
  stream << _("See the man page for more information.\n\n");
  stream << _("Report bugs to <") << PACKAGE_BUGREPORT << ">.\n";
  stream << _("Project home page: <") << PACKAGE_URL << ">.";
  stream << "\n";
}
