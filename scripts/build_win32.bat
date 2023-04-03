@echo off
setlocal
if [%1] == [] goto usage

setup_vst3sdk_win32
if %errorlevel% neq 0 exit /b !errorlevel!
build_infernal_win32 %1
if %errorlevel% neq 0 exit /b !errorlevel!
goto :eof

:usage
echo "Usage: %0 <path-to-hiir>"
exit /B 1
