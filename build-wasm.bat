@echo off

set BUILD_TYPE=Release

:: Reset build directory
rd /s /q build 2>nul
mkdir build & cd build

:: Configure and build
emcmake cmake -G Ninja -DCMAKE_BUILD_TYPE=%BUILD_TYPE% .. & cmake --build . --config %BUILD_TYPE% -- -j %NUMBER_OF_PROCESSORS% & cd ..
