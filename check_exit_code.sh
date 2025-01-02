#!/bin/sh

# Check the exit code of the command. The first argument passed to the script
# is the expected exit code. The rest of the arguments are the command to run.
# If the exit code of the command does not match the expected exit code, the
# script will print an error message and exit with a non-zero exit code.

EXPECTED_EXIT_CODE=$1
shift
"$@"
return_code=$?

if [ $return_code -ne $EXPECTED_EXIT_CODE ]; then
  echo "Expected exit code $EXPECTED_EXIT_CODE but got $return_code"
  exit 1
fi
