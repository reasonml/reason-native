: # This is a special script which intermixes both sh
: # and cmd code. It is written this way because it is
: # used in system() shell-outs directly in otherwise
: # portable code. See https://stackoverflow.com/questions/17510688
: # for details.

cd src/refmterr
refmt ./tests/test.re --print ml > ./tests/test.ml

:; ocaml ./tests/test.ml
:; exit

Powershell -Command 'ocaml ./tests/test.ml'
