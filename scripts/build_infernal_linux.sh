#!/bin/bash
set -e

if [ "$#" -ne 1 ]
then
  echo "Usage: $0 <path-to-hiir>"
  exit 1
fi

cd ..
mkdir -p build/infernal/linux/debug
cd build/infernal/linux/debug
cmake -DCMAKE_BUILD_TYPE=Debug -DHIIR_140_SRC_DIR="$1" ../../../../
make

cd ..
mkdir -p release
cd release
cmake -DCMAKE_BUILD_TYPE=Release -DHIIR_140_SRC_DIR="$1" ../../../../
make
strip --strip-unneeded ../../../../dist/linux/Release/InfernalSynth1.1.vst3/Contents/x86_64-linux/InfernalSynth1.1.so
strip --strip-unneeded ../../../../dist/linux/Release/InfernalSynthGeneric.vst3/Contents/x86_64-linux/InfernalSynthGeneric.so

cd ../../../../scripts
