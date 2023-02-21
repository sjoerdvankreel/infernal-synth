#!/bin/bash

cd ..
mkdir -p build
cd build
mkdir -p linux
cd linux

mkdir -p debug
cd debug
cmake ../../../ -DCMAKE_BUILD_TYPE=Debug -DVST_SDK_376_SRC_DIR=/media/sf_vst-sdk_3.7.6/linux/VST_SDK/vst3sdk -DVST_SDK_376_BUILD_DEBUG_DIR=/media/sf_vst-sdk_3.7.6/linux/VST_SDK/vst3sdk/build/debug/lib/Debug -DVST_SDK_376_BUILD_RELEASE_DIR=/media/sf_vst-sdk_3.7.6/linux/VST_SDK/vst3sdk/build/release/lib/Release -DHIIR_140_SRC_DIR=/media/sf_hiir_1.40
make
cd ..

mkdir -p release
cd release
cmake ../../../ -DCMAKE_BUILD_TYPE=Release -DVST_SDK_376_SRC_DIR=/media/sf_vst-sdk_3.7.6/linux/VST_SDK/vst3sdk -DVST_SDK_376_BUILD_DEBUG_DIR=/media/sf_vst-sdk_3.7.6/linux/VST_SDK/vst3sdk/build/debug/lib/Debug -DVST_SDK_376_BUILD_RELEASE_DIR=/media/sf_vst-sdk_3.7.6/linux/VST_SDK/vst3sdk/build/release/lib/Release -DHIIR_140_SRC_DIR=/media/sf_hiir_1.40
make
cd ..

cd ..
cd ..
cd scripts
