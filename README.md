# About
SevenSynth is a semi-modular software synthesizer and effect plugin.\
For system requirements, download and installation, see the [project website](https://sjoerdvankreel.github.io/seven/).\
\
![Screenshot](static/screenshot.png)

# Legal
<img align="left" alt="VST logo" src="static/vst_logo.png">
VST is a trademark of Steinberg Media Technologies GmbH,<br/>
registered in Europe and other countries.
<br clear="left"/>

# Dependencies
- Laurent de Soras' HIIR library: [http://ldesoras.free.fr/](http://ldesoras.free.fr/).
- Steinberg VST3 SDK: [https://github.com/steinbergmedia/vst3sdk](https://github.com/steinbergmedia/vst3sdk).

# Credits

- Artwork by DanteCyberman: [Seven deadly sins](https://www.deviantart.com/dantecyberman/art/Seven-deadly-sins-442680725).
- Reverb demo adapted from RMB's classic "Spring".
- Contains a slightly adapted implementation of [Jezar's Freeverb](https://github.com/sinshu/freeverb).
- Contains a slightly adapted implementation of the [Karplus-Strong algorithm](https://blog.demofox.org/2016/06/16/synthesizing-a-pluked-string-sound-with-the-karplus-strong-algorithm).
- Contains a verbatim implementation of [Andrew Simper's state variable filter equations](https://cytomic.com/files/dsp/SvfLinearTrapOptimised2.pdf).
- Contains a verbatim implementation of [Moorer's DSF algorithm as described by Burkhard Reike](https://www.verklagekasper.de/synths/dsfsynthesis/dsfsynthesis.html).

Furthermore parts of SevenSynth are implemented based on various resources found mostly on 
[KVR Audio](https://www.kvraudio.com), [DSP Stackexchange](https://dsp.stackexchange.com),
[Music DSP](https://www.musicdsp.org) and [DSP Related](https://www.dsprelated.com).
Look for // http:// or // https:// in the source code for sources relevant to a specific feature.

# How to build

- Download HIIR (see CMakeLists for version): [http://ldesoras.free.fr/prod.html](http://ldesoras.free.fr/prod.html).
- Download and compile VST3 SDK (see CMakeLists for version): [https://github.com/steinbergmedia/vst3sdk](https://github.com/steinbergmedia/vst3sdk).
- Generate Visual Studio project using CMake (example in scripts/make_win32), then compile SevenSynth.
- Run UI generator (example in scripts/gen_vstui), rebuild to make sure output ends up with the plugin binaries.
- Copy /build/win32/Release/SevenSynth to VST3 folder (usually C:\Program Files\Common Files\VST3).
