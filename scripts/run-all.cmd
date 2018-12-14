:; set -e
:; for pkg in *.opam; do
:;   [ -f "$pkg" ] || break
:;   "$@" "${pkg%%.*}"
:; done
for %%f in (*.opam) do (
    %* %%~nf
)