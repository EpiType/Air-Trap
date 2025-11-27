@echo off
REM Build script for Windows

echo Building Air-Trap...

REM Create build directory
if not exist build mkdir build
cd build

REM Install dependencies with Conan
conan install .. --build=missing -s build_type=Release

REM Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

REM Build
cmake --build . --config Release

echo Build complete!
echo Server: .\build\Release\r-type_server.exe
echo Client: .\build\Release\r-type_client.exe

cd ..
