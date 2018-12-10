cd src/refmterr
refmt ./tests/test.re --print ml > ./tests/test.ml
:; set -e
ocaml ./tests/test.ml || exit 0
