cd ..
if not exist "vst3sdk" git clone --recursive https://github.com/steinbergmedia/vst3sdk.git
cd vst3sdk
git checkout v3.7.7_build_19
git submodule set-url -- vstgui4 https://github.com/sjoerdvankreel/vstgui.git
cd vstgui4
git checkout develop
git pull
cd ..
cd ..
if not exist "build" mkdir build
cd build
if not exist "vst3sdk" mkdir vst3sdk
cd vst3sdk 
if not exist "win32" mkdir win32
cd win32
cmake -DVSTGUI_ENABLE_INFERNAL=1 ../../../vst3sdk
msbuild /property:Configuration=Debug vstsdk.sln
msbuild /property:Configuration=Release vstsdk.sln