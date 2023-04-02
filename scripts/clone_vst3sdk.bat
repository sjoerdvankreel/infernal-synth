git clone --recursive https://github.com/steinbergmedia/vst3sdk.git
cd vst3sdk
git submodule set-url -- vstgui4 https://github.com/sjoerdvankreel/vstgui.git
cd vstgui4
git checkout develop
git pull