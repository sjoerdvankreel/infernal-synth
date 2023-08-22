#ifndef IPIS_PLUGIN_HPP
#define IPIS_PLUGIN_HPP
// macro names cannot be too long for resource compiler

#define IPIS_VERSION "1.3"
#define IPIS_VERSION_MAJOR 1
#define IPIS_VERSION_MINOR 3
#define IPIS_VERSION_REVISION 0
#define IPIS_VENDOR_NAME "Sjoerd van Kreel"
#define IPIS_VENDOR_MAIL "sjoerdvankreel@gmail.com"
#define IPIS_VENDOR_URL "https://sjoerdvankreel.github.io/infernal-synth"

#if IPIS_FX
#define IPIS_NAME "InfernalSynthFX 1.3"
#define IPIS_UNIQUE_ID "D1D3802692374AB7B6FC3A55A9AE3BCC"
#elif (!IPIS_FX)
#define IPIS_NAME "InfernalSynth 1.3"
#define IPIS_UNIQUE_ID "5626A8A247C740E3895EF722E6C1D9C4"
#else 
#error
#endif

#endif // IPIS_PLUGIN_HPP