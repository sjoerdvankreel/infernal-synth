#ifndef INF_SYNTH_VST_PLUGIN_HPP
#define INF_SYNTH_VST_PLUGIN_HPP

// Don't forget to update the versioned guids, too, if breaking changes are introduced.
#define INF_SYNTH_VST_VERSION "1.3"
#define INF_SYNTH_VST_VERSION_MAJOR 1
#define INF_SYNTH_VST_VERSION_MINOR 3
#define INF_SYNTH_VST_VERSION_REVISION 0

#define INF_SYNTH_VENDOR_NAME "Sjoerd van Kreel"

#if INF_FX
#if INF_VERSIONED
#define INF_SYNTH_VST_NAME "InfernalSynth FX 1.3"
#else
#define INF_SYNTH_VST_NAME "InfernalSynth FX Generic"
#endif // versioned
#define INF_SYNTH_VST_CONTROLLER_NAME "InfernalSynthFXController"
#else // fx
#if INF_VERSIONED
#define INF_SYNTH_VST_NAME "InfernalSynth 1.3"
#else
#define INF_SYNTH_VST_NAME "InfernalSynth Generic"
#endif // versioned
#define INF_SYNTH_VST_CONTROLLER_NAME "InfernalSynthController"
#endif // fx

#define INF_SYNTH_VST_COMPANY_WEB "Infernal"
#define INF_SYNTH_VST_COMPANY_NAME "Infernal"
#define INF_SYNTH_VST_ORIG_FILE_NAME "InfernalSynth.vst3"
#define INF_SYNTH_VST_LEGAL_COPYRIGHT "(C) Sjoerd van Kreel"
#define INF_SYNTH_VST_COMPANY_MAIL "sjoerdvankreel@gmail.com"
#define INF_SYNTH_VST_LEGAL_TRADEMARKS "VST is a trademark of Steinberg Media Technologies GmbH"

#endif // INF_SYNTH_VST_PLUGIN_HPP
