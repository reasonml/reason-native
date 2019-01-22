#! /bin/bash

rm -rf _release
esy i &&
esy b &&
esy npm-release &&
cd _release &&
npm pack &&
npm i -g ./pesy-0.0.0.tgz &&
cd ..
./_build/install/default/bin/TestPesyConfigure.exe

