#ifndef IPISFV3_PLUGIN_HPP
#define IPISFV3_PLUGIN_HPP

#include <inf.plugin.infernal_synth/plugin.hpp>

#if IPISFV3_FX
#define IPISFV3_CONTROLLER_NAME "InfernalSynthFXController"
#elif (!IPISFV3_FX)
#define IPISFV3_CONTROLLER_NAME "InfernalSynthController"
#else 
#error
#endif

#endif // IPISFV3_PLUGIN_HPP