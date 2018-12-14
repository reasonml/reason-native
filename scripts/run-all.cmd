:; set -e
:; for folder in src/*/; do
:;   "$@" $(basename $folder)
:; done
for /D %%f in (src/*) do (
    %* %%f
)