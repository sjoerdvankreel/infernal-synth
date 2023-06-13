#!/bin/bash
set -e

if [ "$#" -ne 5 ]
then
  echo "Usage: $0 <path-to-hiir-src> <path-to-vst3sdk-src> <path-to-vst3sdk-debug-binaries> <path-to-vst3sdk-release-binaries> <linux-distro-name>"
  exit 1
fi

cd ..
mkdir -p build/linux_"$5"/debug
cd build/linux_"$5"/debug
cmake -DCMAKE_BUILD_TYPE=Debug -DINFERNAL_LINUX_DISTRO="$5" -DHIIR_140_SRC_DIR="$1" DVST3_SDK_378_SRC_DIR="$2" -DVST3_SDK_378_BUILD_DEBUG_DIR="$3" -DVST3_SDK_378_BUILD_RELEASE_DIR="$4" ../../..
make

cd ..
mkdir -p release
cd release
cmake -DCMAKE_BUILD_TYPE=Release -DINFERNAL_LINUX_DISTRO="$5" -DHIIR_140_SRC_DIR="$1" -DVST3_SDK_378_SRC_DIR="$2" -DVST3_SDK_378_BUILD_DEBUG_DIR="$3" -DVST3_SDK_378_BUILD_RELEASE_DIR="$4" ../../..
make

cd ../../../scripts
