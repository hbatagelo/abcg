@echo off

set BUILD_TYPE=Debug

:: Reset build directory
rd /s /q build 2>nul
mkdir build & cd build

:: Configure
cmake -GNinja -DCMAKE_BUILD_TYPE=%BUILD_TYPE% ^
              -DCMAKE_CXX_COMPILER=g++ ^
              -DCMAKE_C_COMPILER=gcc ^
              ..

:: Build
cmake --build . --config %BUILD_TYPE% -- -j %NUMBER_OF_PROCESSORS%

cd ..
