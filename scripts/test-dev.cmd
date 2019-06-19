:; set -e

:; # This will be the location of this script.
:; DIR="$(cd "$(dirname "$0")" && pwd)"

:; # Run ReasonNativeTests.exe with correct root set.
:; REASON_NATIVE_ROOT="$DIR/../" esy x "TestDev.exe" --onlyPrintDetailsForFailedSuites "$@"
