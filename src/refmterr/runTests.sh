#!/bin/sh

echo $cur__name
if [[ "$cur__name" != "reason-native-dev" ]]; then
  echo "You must run the tests in the correct sandbox. Do:"
  echo ""
  echo "  esy src/refmterr/runTests.sh"
  echo ""
  exit 1
fi

cd src/refmterr

refmt ./tests/test.re --print ml > ./tests/test.ml
ocaml ./tests/test.ml
