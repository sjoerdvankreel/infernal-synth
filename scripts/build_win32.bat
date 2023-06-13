@echo off
setlocal
if [%1] == [] goto usage
if [%2] == [] goto usage
if [%3] == [] goto usage
if [%4] == [] goto usage

cd ..
if not exist build\win32 mkdir build\win32
cd build\win32
cmake -DHIIR_140_SRC_DIR="%1" -DVST3_SDK_378_SRC_DIR="%2" -DVST3_SDK_378_BUILD_DEBUG_DIR="%3" -DVST3_SDK_378_BUILD_RELEASE_DIR="%4" ../..
if %errorlevel% neq 0 exit /b !errorlevel!

msbuild /property:Configuration=Debug infernal-synth.sln
if %errorlevel% neq 0 exit /b !errorlevel!
msbuild /property:Configuration=Release infernal-synth.sln
if %errorlevel% neq 0 exit /b !errorlevel!

cd ..\..\scripts
goto :eof

:usage
echo "Usage: %0 <path-to-hiir-src> <path-to-vst3sdk-src> <path-to-vst3sdk-debug-binaries> <path-to-vst3sdk-release-binaries>"
exit /B 1
