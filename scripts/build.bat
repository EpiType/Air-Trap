@echo off

echo Building Air-Trap...

if not exist build mkdir build
cd build

conan install .. --output-folder=. --build=missing -s build_type=Release -s compiler.cppstd=23

cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release

cmake --build . --config Release

echo Build complete!

echo Server: .\Release\r-type_server.exe
echo Client: .\Release\r-type_client.exe

cd ..