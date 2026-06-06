cmake -S . -B "Baloker Online/build/debug" -DCMAKE_C_COMPILER=gcc -DCMAKE_BUILD_TYPE=Debug
cmake --build '.\Baloker Online\build\debug'

cmake -S . -B "Baloker Online/build/release" -DCMAKE_C_COMPILER=gcc -DCMAKE_BUILD_TYPE=Release
cmake --build '.\Baloker Online\build\release'
