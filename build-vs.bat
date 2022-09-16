@echo off

if "%1"=="" ( set BUILD_TYPE=Release ) else ( set BUILD_TYPE=%1 )
set CMAKE_EXTRA_ARGS=%2

:: Reset build directory
rd /s /q build 2>nul
mkdir build & cd build

:: Configure
cmake -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=%BUILD_TYPE% %CMAKE_EXTRA_ARGS% ..

:: Build
cmake --build . --config %BUILD_TYPE%

cd ..
