#!/bin/bash
set -e

if [ "$#" -ne 5 ]
then
  echo "Usage: $0 <path-to-hiir-src> <path-to-juce-src> <path-to-vst3sdk-src> <path-to-vst3sdk-debug-binaries> <path-to-vst3sdk-release-binaries>"
  exit 1
fi

cd ..
mkdir -p build/linux/debug
cd build/linux/debug
cmake -DCMAKE_BUILD_TYPE=Debug -DHIIR_140_SRC_DIR="$1" -DJUCE_705_SRC_DIR="$2" -DVST3_SDK_378_SRC_DIR="$3" -DVST3_SDK_378_BUILD_DEBUG_DIR="$4" -DVST3_SDK_378_BUILD_RELEASE_DIR="$5" ../../..
make

cd ..
mkdir -p release
cd release
cmake -DCMAKE_BUILD_TYPE=Release -DHIIR_140_SRC_DIR="$1" -DJUCE_705_SRC_DIR="$2" -DVST3_SDK_378_SRC_DIR="$3" -DVST3_SDK_378_BUILD_DEBUG_DIR="$4" -DVST3_SDK_378_BUILD_RELEASE_DIR="$5" ../../..
make
#strip --strip-unneeded ../../../dist/linux/Release/InfernalSynth1.2.vst3/Contents/x86_64-linux/InfernalSynth1.2.so
#strip --strip-unneeded ../../../dist/linux/Release/InfernalSynthGeneric.vst3/Contents/x86_64-linux/InfernalSynthGeneric.so

cd ../../../scripts
