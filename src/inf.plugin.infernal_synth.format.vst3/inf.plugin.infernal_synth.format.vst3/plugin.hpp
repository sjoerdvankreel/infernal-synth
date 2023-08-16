#ifndef IPISFV3_PLUGIN_HPP
#define IPISFV3_PLUGIN_HPP
// macro names cannot be too long for resource compiler

#define IPISFV3_VERSION "1.3"
#define IPISFV3_VERSION_MAJOR 1
#define IPISFV3_VERSION_MINOR 3
#define IPISFV3_VERSION_REVISION 0
#define IPISFV3_VENDOR_NAME "Sjoerd van Kreel"

#if IPISFV3_FX
#define IPISFV3_NAME "InfernalSynthFX 1.3"
#define IPISFV3_CONTROLLER_NAME "InfernalSynthFXController"
#elif (!IPISFV3_FX)
#define IPISFV3_NAME "InfernalSynth 1.3"
#define IPISFV3_CONTROLLER_NAME "InfernalSynthController"
#else 
#error
#endif

#define IPISFV3_COMPANY_WEB "Infernal"
#define IPISFV3_COMPANY_NAME "Infernal"
#define IPISFV3_ORIG_FILE_NAME "InfernalSynth.vst3"
#define IPISFV3_LEGAL_COPYRIGHT "(C) Sjoerd van Kreel"
#define IPISFV3_COMPANY_MAIL "sjoerdvankreel@gmail.com"
#define IPISFV3_LEGAL_TRADEMARKS "VST is a trademark of Steinberg Media Technologies GmbH"

#endif // IPISFV3_PLUGIN_HPP
