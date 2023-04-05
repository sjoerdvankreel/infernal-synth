@echo off
setlocal
if [%1] == [] goto usage

cd ..
if not exist build\infernal\win32 mkdir build\infernal\win32
cd build\infernal\win32
cmake -DHIIR_140_SRC_DIR="%1" ../../../
if %errorlevel% neq 0 exit /b !errorlevel!

msbuild /property:Configuration=Debug infernal-synth.sln
if %errorlevel% neq 0 exit /b !errorlevel!
msbuild /property:Configuration=Release infernal-synth.sln
if %errorlevel% neq 0 exit /b !errorlevel!

cd ..\..\..\scripts
goto :eof

:usage
echo "Usage: %0 <path-to-hiir>"
exit /B 1
