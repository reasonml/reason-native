cd src/refmterr
refmt ./tests/test.re --print ml > ./tests/test.ml
which refmttype
:; set -e
ocaml ./tests/test.ml || exit 0
