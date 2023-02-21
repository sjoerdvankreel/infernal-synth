#ifndef SVN_SYNTH_OSCILLATOR_CONFIG_HPP
#define SVN_SYNTH_OSCILLATOR_CONFIG_HPP

#include <cstdint>

namespace inf::synth {

inline std::int32_t constexpr osc_sync_off = 0;
inline std::int32_t constexpr osc_max_voices = 8;
inline std::int32_t constexpr osc_sync_fade_length = 16;
inline std::int32_t constexpr osc_noise_color_bin_count = 16;

} // namespace inf::synth
#endif // SVN_SYNTH_OSCILLATOR_CONFIG_HPP