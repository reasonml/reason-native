:; set -e
:; node ./scripts/esy-prepublish.js $1.json
:; rm -rf ./_release
:; exit 0

node ./scripts/esy-prepublish.js %1.json || exit 1
rd /S /Q _release
