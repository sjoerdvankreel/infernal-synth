@echo off
cd ..
mkdir build
cd build
mkdir win32
cd win32
cmake ../../ -DVST_SDK_377_SRC_DIR=C:/repos/vst-sdk_3.7.7/vst3sdk -DVST_SDK_377_BUILD_DEBUG_DIR=C:/repos/vst-sdk_3.7.7/win32/lib/Debug -DVST_SDK_377_BUILD_RELEASE_DIR=C:/repos/vst-sdk_3.7.7/win32/lib/Release -DVSTGUI_FORK_SRC_DIR=C:/repos/vstgui_fork/vstgui -DVSTGUI_FORK_BUILD_DEBUG_DIR=C:/repos/vstgui_fork/win32/Debug/libs -DVSTGUI_FORK_BUILD_RELEASE_DIR=C:/repos/vstgui_fork/win32/Release/libs -DHIIR_140_SRC_DIR=C:/repos/hiir_1.40
cd ..
cd ..
cd scripts
set /p wait=Press return...
