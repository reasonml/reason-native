set -e
[ -e ./src/refmterr/lib/common ] && rm -rf ./src/refmterr/lib/common
cp -r ./common ./src/refmterr/lib/common
[ -e ./src/rely/library/common ] && rm -rf ./src/rely/library/common
cp -r ./common ./src/rely/library/common
