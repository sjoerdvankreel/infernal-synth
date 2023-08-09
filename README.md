# About
InfernalSynth is a semi-modular software synthesizer and effect plugin.\
For system requirements, download and installation, see the [project website](https://sjoerdvankreel.github.io/infernal-synth).

# Legal
<img align="left" alt="VST logo" src="static/vst_logo.png">
VST is a trademark of Steinberg Media Technologies GmbH,<br/>
registered in Europe and other countries.
<br clear="left"/>

# Dependencies
- JUCE: [https://github.com/juce-framework/JUCE](https://github.com/juce-framework/JUCE).
- Laurent de Soras' HIIR library: [http://ldesoras.free.fr/](http://ldesoras.free.fr/).
- Steinberg VST3 SDK: [https://github.com/steinbergmedia/vst3sdk](https://github.com/steinbergmedia/vst3sdk).

# Credits
- Special thanks to host vendors who have set me up with an NFR license.
- Special thanks to Surge Synth Team: [https://github.com/surge-synthesizer/surge](https://github.com/surge-synthesizer/surge).
- Artwork by DanteCyberman: [Seven deadly sins](https://www.deviantart.com/dantecyberman/art/Seven-deadly-sins-442680725).
- Contains a slightly adapted implementation of [Jezar's Freeverb](https://github.com/sinshu/freeverb).
- Contains a slightly adapted implementation of the [Karplus-Strong algorithm](https://blog.demofox.org/2016/06/16/synthesizing-a-pluked-string-sound-with-the-karplus-strong-algorithm).
- Contains a verbatim implementation of [Andrew Simper's state variable filter equations](https://cytomic.com/files/dsp/SvfLinearTrapOptimised2.pdf).
- Contains a verbatim implementation of [Moorer's DSF algorithm as described by Burkhard Reike](https://www.verklagekasper.de/synths/dsfsynthesis/dsfsynthesis.html).

Furthermore parts of InfernalSynth are implemented based on various resources found mostly on
[KVR Audio](https://www.kvraudio.com), [DSP Stackexchange](https://dsp.stackexchange.com),
[Music DSP](https://www.musicdsp.org) and [DSP Related](https://www.dsprelated.com).
Look for // http:// or // https:// in the source code for sources relevant to a specific feature.

# How to build
- Needs cmake and visual studio 2022
- Source dependencies are included in the /lib folder
- Clone infernal synth into /path/to/infernal
- Run /path/to/infernal/scripts/build_win32.bat
- If all goes well, final output is produced in /path/to/infernal/dist.

# Screenshots
See screenshots for all themes: [https://sjoerdvankreel.github.io/infernal-synth/screenshots.html](https://sjoerdvankreel.github.io/infernal-synth/screenshots.html).\
![Screenshot](static/screenshot_1.png)
![Screenshot](static/screenshot_2.png)
![Screenshot](static/screenshot_3.png)
![Screenshot](static/screenshot_4.png)