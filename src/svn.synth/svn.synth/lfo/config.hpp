#ifndef SVN_SYNTH_LFO_CONFIG_HPP
#define SVN_SYNTH_LFO_CONFIG_HPP

#include <cstdint>

namespace svn::synth {

inline float constexpr lfo_min_rate = 0.05f;
inline float constexpr lfo_max_rate = 20.0f;

inline std::int32_t constexpr lfo_slope_range = 8;
inline std::int32_t constexpr lfo_rand_min_steps = 2;
inline std::int32_t constexpr lfo_rand_max_steps = 32;
inline std::int32_t constexpr lfo_rand_default_steps = 4;

} // namespace svn::synth
#endif // SVN_SYNTH_LFO_CONFIG_HPP