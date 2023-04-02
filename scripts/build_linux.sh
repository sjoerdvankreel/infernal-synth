#!/bin/bash
set -e

cd ..
mkdir -p build/infernal/linux/debug
cd build/infernal/linux/debug
cmake -DCMAKE_BUILD_TYPE=Debug -DVSTGUI_ENABLE_INFERNAL=1 -DHIIR_140_SRC_DIR="$1" ../../../../
make

cd ../../../../
mkdir -p build/infernal/linux/release
cd build/infernal/linux/release
cmake -DCMAKE_BUILD_TYPE=Release -DVSTGUI_ENABLE_INFERNAL=1 -DHIIR_140_SRC_DIR="$1" ../../../../
make
strip --strip-unneeded ../../../../dist/Release/InfernalSynth1.1.vst3/Contents/x86_64-linux/InfernalSynth1.1.so
strip --strip-unneeded ../../../../dist/Release/InfernalSynthGeneric.vst3/Contents/x86_64-linux/InfernalSynthGeneric.so

cd ../../../../scripts
