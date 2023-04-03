@echo off
if [%1] == [] goto usage

setup_vst3sdk_win32
build_infernal_win32 %1
goto :eof

:usage
echo "Usage: %0 <path-to-hiir>"
exit /B 1
