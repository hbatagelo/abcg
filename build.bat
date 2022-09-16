@echo off

if "%1"=="" ( set BUILD_TYPE=Release  ) else ( set BUILD_TYPE=%1 )
set CMAKE_EXTRA_ARGS=%2

:: Reset build directory
rd /s /q build 2>nul
mkdir build & cd build

:: Configure
cmake -GNinja -DCMAKE_BUILD_TYPE=%BUILD_TYPE% ^
              -DCMAKE_CXX_COMPILER=g++ ^
              -DCMAKE_C_COMPILER=gcc ^
              %CMAKE_EXTRA_ARGS% ^
              ..

:: Build
cmake --build . --config %BUILD_TYPE% -- -j %NUMBER_OF_PROCESSORS%

cd ..
