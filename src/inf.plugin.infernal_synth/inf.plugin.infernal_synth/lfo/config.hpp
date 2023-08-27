#ifndef INF_PLUGIN_INFERNAL_SYNTH_LFO_CONFIG_HPP
#define INF_PLUGIN_INFERNAL_SYNTH_LFO_CONFIG_HPP

#include <cstdint>

namespace inf::plugin::infernal_synth {

inline float constexpr lfo_min_rate = 0.05f;
inline float constexpr lfo_max_rate = 20.0f;

inline std::int32_t constexpr lfo_slope_range = 8;
inline std::int32_t constexpr lfo_rand_min_steps = 2;
inline std::int32_t constexpr lfo_rand_max_steps = 32;
inline std::int32_t constexpr lfo_rand_default_steps = 4;

} // namespace inf::plugin::infernal_synth
#endif // INF_PLUGIN_INFERNAL_SYNTH_LFO_CONFIG_HPP