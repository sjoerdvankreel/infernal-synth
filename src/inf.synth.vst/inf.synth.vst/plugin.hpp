#ifndef INF_SYNTH_VST_PLUGIN_HPP
#define INF_SYNTH_VST_PLUGIN_HPP

// Don't forget to update the versioned guids, too, if breaking changes are introduced.
#define INF_SYNTH_VST_VERSION "1.2"
#define INF_SYNTH_VST_VERSION_MAJOR 1
#define INF_SYNTH_VST_VERSION_MINOR 2
#define INF_SYNTH_VST_VERSION_REVISION 0
#if INF_VERSIONED
#define INF_SYNTH_VST_FX_NAME "InfernalSynth FX 1.2"
#define INF_SYNTH_VST_INSTRUMENT_NAME "InfernalSynth 1.2"
#elif !INF_VERSIONED
#define INF_SYNTH_VST_FX_NAME "InfernalSynth FX"
#define INF_SYNTH_VST_INSTRUMENT_NAME "InfernalSynth Generic"
#else
#error
#endif

#define INF_SYNTH_VST_COMPANY_WEB "Infernal"
#define INF_SYNTH_VST_COMPANY_NAME "Infernal"
#define INF_SYNTH_VST_ORIG_FILE_NAME "InfernalSynth.vst3"
#define INF_SYNTH_VST_LEGAL_COPYRIGHT "(C) Sjoerd van Kreel"
#define INF_SYNTH_VST_COMPANY_MAIL "sjoerdvankreel@gmail.com"
#define INF_SYNTH_VST_FX_CONTROLLER_NAME "InfernalSynthFXController"
#define INF_SYNTH_VST_INSTRUMENT_CONTROLLER_NAME "InfernalSynthController"
#define INF_SYNTH_VST_LEGAL_TRADEMARKS "VST is a trademark of Steinberg Media Technologies GmbH"

#endif // INF_SYNTH_VST_PLUGIN_HPP
