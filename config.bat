#!/bin/bash

mkdir lib

cd ./external/rl_imgui/

chmod +x premake5

./premake5 gmake

make

cd ../..

cp ./external/rl_imgui/bin/Debug/rlImGui.lib ./lib/rlImGui.lib

cd ./external/raylib/src

make PLATFORM=PLATFORM_DESKTOP

cd ../../..

cp ./external/raylib/src/libraylib.a ./lib/libraylib.a

cd ./external/enet/

cmake -G "MinGW Makefiles" CMakeLists.txt --fresh

make

cd ../../

cp ./external/enet/libenet.a ./lib/libenet.a

cp ./make_windows ./Makefile