cd src/refmterr
refmt ./tests/test.re --print ml > ./tests/test.ml
ls ../../
echo "$(npm config get prefix)/lib/node_modules"
ocaml ./tests/test.ml
