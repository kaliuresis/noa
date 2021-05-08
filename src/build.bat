@echo off

pushd "..\generated"

emcc -s EXPORTED_FUNCTIONS="['_Random', '_SetRandomSeed', '_search_spiral_start', '_search_spiral_step', '_search_portal_start', '_search_portal_step']" -s EXPORTED_RUNTIME_METHODS="['ccall','cwrap']" -Wno-c++11-narrowing -Wno-writable-strings --source-map-base -O3 -g4 -I "./" -D DEBUG "../src/noita_random.cpp" -o "noita_random.js"

popd
