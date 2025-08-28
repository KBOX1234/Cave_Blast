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

cd ./external/sphysics/

make

cd ../../

cp ./external/sphysics/libsphysics.a ./lib/libsphysics.a

git clone https://github.com/bkaradzic/GENie.git

cd GENie

make

cd ../

cp ./GENie/bin/windows/genie.exe ./

rm -rf GENie

export PATH="$(pwd):$PATH"

cd ./external/soloud/build/

genie gmake "SOLOUD_NO_OPENAL=1" "SOLOUD_NO_SDL2=1" "SOLOUD_NO_PORTAUDIO=1"

cd gmake

make SoloudStatic LDFLAGS="-lwinmm"

cd ../../../../

cp ./external/soloud/lib/libsoloud_static.a ./lib/libsoloud.a

rm -rf genie.exe