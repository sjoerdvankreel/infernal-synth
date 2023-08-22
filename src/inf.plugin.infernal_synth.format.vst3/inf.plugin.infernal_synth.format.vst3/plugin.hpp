#ifndef IPISFV3_PLUGIN_HPP
#define IPISFV3_PLUGIN_HPP

#include <inf.plugin.infernal_synth/plugin.hpp>

#if (!defined IPISFV3_FX)
#error
#endif
#if IPISFV3_FX
#define IPISFV3_NAME IPIS_FX_NAME
#define IPISFV3_UNIQUE_ID IPIS_FX_UNIQUE_ID
#define IPISFV3_CONTROLLER_NAME "InfernalSynthFXController"
#else
#define IPISFV3_NAME IPIS_INST_NAME
#define IPISFV3_UNIQUE_ID IPIS_INST_UNIQUE_ID
#define IPISFV3_CONTROLLER_NAME "InfernalSynthController"
#endif

#endif // IPISFV3_PLUGIN_HPP
