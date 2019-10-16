:; set -e

:; # This will be the location of this script.
:; DIR="$(cd "$(dirname "$0")" && pwd)"

:; rm -rf _coverage

:; # Run ReasonNativeTests.exe with correct root set.
:; REASON_NATIVE_ROOT="$DIR/../" esy @coverage x "TestCi.exe" "$@"

:; esy bisect-ppx-report html

:; rm -rf *.coverage
