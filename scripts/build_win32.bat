cd ..
if not exist build\infernal\win32 mkdir build\infernal\win32
cd build\infernal\win32
cmake -DVSTGUI_ENABLE_INFERNAL=1 -DHIIR_140_SRC_DIR="%1" ../../../
msbuild /property:Configuration=Debug infernal-synth.sln
msbuild /property:Configuration=Release infernal-synth.sln
cd ..\..\..\scripts