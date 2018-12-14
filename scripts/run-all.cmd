:; set -e
:; for folder in src/*/; do
:;   "$@" $(basename $folder)
:; done
:; exit 0
for /D %%f in (src/*) do (
    %* %%f || exit 1
)