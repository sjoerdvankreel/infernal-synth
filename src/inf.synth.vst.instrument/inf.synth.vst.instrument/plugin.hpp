#ifndef INF_SYNTH_VST_PLUGIN_HPP
#define INF_SYNTH_VST_PLUGIN_HPP

// Don't forget to update the versioned guids, too.
#define INF_SYNTH_VST_VERSION "1.1.2"
#define INF_SYNTH_VST_VERSION_MAJOR 1
#define INF_SYNTH_VST_VERSION_MINOR 1
#define INF_SYNTH_VST_VERSION_REVISION 2
#define INF_SYNTH_VST_ORIG_FILE_NAME "InfernalSynth.vst3"
#define INF_SYNTH_VST_LEGAL_COPYRIGHT "(C) Sjoerd van Kreel"
#define INF_SYNTH_VST_LEGAL_TRADEMARKS "VST is a trademark of Steinberg Media Technologies GmbH"
#if INF_VERSIONED
#define INF_SYNTH_VST_NAME "InfernalSynth 1.1.2"
#elif !INF_VERSIONED
#define INF_SYNTH_VST_NAME "InfernalSynth Generic"
#else
#error
#endif

#define INF_SYNTH_VST_COMPANY_WEB "Infernal"
#define INF_SYNTH_VST_COMPANY_NAME "Infernal"
#define INF_SYNTH_VST_COMPANY_MAIL "sjoerdvankreel@gmail.com"
#define INF_SYNTH_VST_CONTROLLER_NAME "InfernalSynthController"

#endif // INF_SYNTH_VST_PLUGIN_HPP
