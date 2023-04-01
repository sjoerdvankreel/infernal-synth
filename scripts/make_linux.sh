#!/bin/bash

cd ..
mkdir -p build
cd build
mkdir -p linux
cd linux

mkdir -p debug
cd debug
cmake ../../../ -DCMAKE_BUILD_TYPE=Debug -DVST_SDK_377_SRC_DIR=/media/sf_vst-sdk_3.7.7/vst3sdk -DVST_SDK_377_BUILD_DEBUG_DIR=/media/sf_vst-sdk_3.7.7/linux_debug/lib/Debug -DVST_SDK_377_BUILD_RELEASE_DIR=/media/sf_vst-sdk_3.7.7/linux_release/lib/Release -DCMAKE_BUILD_TYPE=Debug -DVSTGUI_FORK_SRC_DIR=/media/sf_vstgui_fork/vstgui -DVSTGUI_FORK_BUILD_DEBUG_DIR=/media/sf_vstgui_fork/linux_debug/Debug/libs -DVSTGUI_FORK_BUILD_RELEASE_DIR=/media/sf_vstgui_fork/linux_release/Release/libs -DHIIR_140_SRC_DIR=/media/sf_hiir_1.40
make
cd ..

mkdir -p release
cd release
cmake ../../../ -DCMAKE_BUILD_TYPE=Release -DVST_SDK_377_SRC_DIR=/media/sf_vst-sdk_3.7.7/vst3sdk -DVST_SDK_377_BUILD_DEBUG_DIR=/media/sf_vst-sdk_3.7.7/linux_debug/lib/Debug -DVST_SDK_377_BUILD_RELEASE_DIR=/media/sf_vst-sdk_3.7.7/linux_release/lib/Release -DCMAKE_BUILD_TYPE=Debug -DVSTGUI_FORK_SRC_DIR=/media/sf_vstgui_fork/vstgui -DVSTGUI_FORK_BUILD_DEBUG_DIR=/media/sf_vstgui_fork/linux_debug/Debug/libs -DVSTGUI_FORK_BUILD_RELEASE_DIR=/media/sf_vstgui_fork/linux_release/Release/libs -DHIIR_140_SRC_DIR=/media/sf_hiir_1.40
make
cd ..

cd ..
cd ..
cd scripts
