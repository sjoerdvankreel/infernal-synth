#!/bin/bash

cd ..
mkdir -p build
cd build
mkdir -p linux
cd linux

mkdir -p debug
cd debug
cmake ../../../ -DCMAKE_BUILD_TYPE=Debug -DVST_SDK_377_SRC_DIR=/media/sf_vst-sdk_3.7.7/linux/vst3sdk_dynamic -DVST_SDK_377_BUILD_DEBUG_DIR=/media/sf_vst-sdk_3.7.7/linux/vst3sdk_dynamic/build/debug/lib -DVST_SDK_377_BUILD_RELEASE_DIR=/media/sf_vst-sdk_3.7.7/linux/vst3sdk_dynamic/build/release/lib -DHIIR_140_SRC_DIR=/media/sf_hiir_1.40
make
cd ..

mkdir -p release
cd release
cmake ../../../ -DCMAKE_BUILD_TYPE=Release -DVST_SDK_377_SRC_DIR=/media/sf_vst-sdk_3.7.7/linux/vst3sdk_dynamic -DVST_SDK_377_BUILD_DEBUG_DIR=/media/sf_vst-sdk_3.7.7/linux/vst3sdk_dynamic/build/debug/lib -DVST_SDK_377_BUILD_RELEASE_DIR=/media/sf_vst-sdk_3.7.7/linux/vst3sdk_dynamic/build/release/lib -DHIIR_140_SRC_DIR=/media/sf_hiir_1.40
make
cd ..

cd ..
cd ..
cd scripts
