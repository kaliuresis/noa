pushd "..\generated"
emcc -s EXPORTED_FUNCTIONS="['_search_spiral_start', '_search_spiral_step']" -s EXPORTED_RUNTIME_METHODS="['ccall','cwrap']" -Wno-c++11-narrowing -Wno-writable-strings --source-map-base -O3 -g4 -I "./" -D DEBUG "../src/main.cpp" -o "noita_random.js"

popd
pause