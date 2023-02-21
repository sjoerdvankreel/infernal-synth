@echo off
cd ..
mkdir build
cd build
mkdir win32
cd win32
cmake ../../ -DVST_SDK_376_SRC_DIR=C:/repos/vst-sdk_3.7.6/win32/VST_SDK/vst3sdk -DVST_SDK_376_BUILD_DEBUG_DIR=C:/repos/vst-sdk_3.7.6/win32/VST_SDK/vst3sdk/build/lib/Debug -DVST_SDK_376_BUILD_RELEASE_DIR=C:/repos/vst-sdk_3.7.6/win32/VST_SDK/vst3sdk/build/lib/Release -DHIIR_140_SRC_DIR=C:/repos/hiir_1.40
cd ..
cd ..
cd scripts
set /p wait=Press return...
