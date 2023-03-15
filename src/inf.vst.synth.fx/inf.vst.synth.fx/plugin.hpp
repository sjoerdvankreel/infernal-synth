#ifndef INF_VST_SYNTH_FX_PLUGIN_HPP
#define INF_VST_SYNTH_FX_PLUGIN_HPP

// Don't forget to update the versioned guids, too.
#define INF_VST_INFERNAL_SYNTH_VERSION "1.1.2"
#define INF_VST_INFERNAL_SYNTH_VERSION_MAJOR 1
#define INF_VST_INFERNAL_SYNTH_VERSION_MINOR 1
#define INF_VST_INFERNAL_SYNTH_VERSION_REVISION 2
#define INF_VST_INFERNAL_SYNTH_ORIG_FILE_NAME "InfernalSynthFX.vst3"
#define INF_VST_INFERNAL_SYNTH_LEGAL_COPYRIGHT "(C) Sjoerd van Kreel"
#define INF_VST_INFERNAL_SYNTH_LEGAL_TRADEMARKS "VST is a trademark of Steinberg Media Technologies GmbH"
#if INF_VERSIONED
#define INF_VST_INFERNAL_SYNTH_NAME "InfernalSynth FX 1.1.2"
#elif !INF_VERSIONED
#define INF_VST_INFERNAL_SYNTH_NAME "InfernalSynth FX Generic"
#else
#error
#endif

#define INF_VST_INFERNAL_SYNTH_COMPANY_WEB "Infernal"
#define INF_VST_INFERNAL_SYNTH_COMPANY_NAME "Infernal"
#define INF_VST_INFERNAL_SYNTH_COMPANY_MAIL "sjoerdvankreel@gmail.com"
#define INF_VST_INFERNAL_SYNTH_CONTROLLER_NAME "InfernalSynthFxController"

#endif // INF_VST_SYNTH_FX_PLUGIN_HPP
