set -e
node ./scripts/esy-prepublish.js $1.json
rm -rf ./_release
