@echo off
if [%1] == [] goto usage

cd ..
if not exist build\infernal\win32 mkdir build\infernal\win32
cd build\infernal\win32
cmake -DHIIR_140_SRC_DIR="%1" ../../../
msbuild /property:Configuration=Debug infernal-synth.sln
msbuild /property:Configuration=Release infernal-synth.sln
cd ..\..\..\scripts
goto :eof

:usage
echo "Usage: build_win32 <path-to-hiir>"
exit /B 1
