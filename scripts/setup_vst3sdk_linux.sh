#!/bin/bash
set -e

#git config --global --add safe.directory '*'

cd ..
if [ ! -d "vst3sdk" ]; then
  git clone --recursive https://github.com/steinbergmedia/vst3sdk.git
fi
cd vst3sdk
git checkout v3.7.7_build_19
git submodule set-url -- vstgui4 https://github.com/sjoerdvankreel/vstgui.git
cd vstgui4
git checkout develop
git pull
cd ../..
mkdir -p build/vst3sdk/linux
cd build/vst3sdk/linux
mkdir -p debug
cd debug
cmake -DCMAKE_BUILD_TYPE=Debug -DVSTGUI_ENABLE_INFERNAL=1 ../../../../vst3sdk
make
cd ..
mkdir -p release
cd release
cmake -DCMAKE_BUILD_TYPE=Release -DVSTGUI_ENABLE_INFERNAL=1 ../../../../vst3sdk
make
cd ../../../../scripts
