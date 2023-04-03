@echo off

cd ..
if not exist "vst3sdk" git clone --recursive https://github.com/steinbergmedia/vst3sdk.git
cd vst3sdk
git checkout v3.7.7_build_19
git submodule set-url -- vstgui4 https://github.com/sjoerdvankreel/vstgui.git
cd vstgui4
git checkout infernal
git pull
cd ..\..

if not exist "build\vst3sdk\win32" mkdir build\vst3sdk\win32
cd build\vst3sdk\win32
cmake -DCMAKE_CXX_FLAGS="-DVSTGUI_ENABLE_INFERNAL=1" -DSMTG_ADD_VST3_PLUGINS_SAMPLES=OFF -DSMTG_ADD_VST3_HOSTING_SAMPLES=OFF -DSMTG_ADD_VST3_UTILITIES=OFF -DSMTG_CREATE_MODULE_INFO=OFF -DSMTG_CREATE_PLUGIN_LINK=OFF -DSMTG_RUN_VST_VALIDATOR=OFF -DVSTGUI_ENABLE_XMLPARSER=OFF ../../../vst3sdk
msbuild /property:Configuration=Debug vstsdk.sln
msbuild /property:Configuration=Release vstsdk.sln
cd ..\..\..\scripts