cd src/refmterr
refmt ./tests/test.re --print ml > ./tests/test.ml
ocaml ./tests/test.ml



