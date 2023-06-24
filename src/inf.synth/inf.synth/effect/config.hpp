#ifndef INF_SYNTH_EFFECT_CONFIG_HPP
#define INF_SYNTH_EFFECT_CONFIG_HPP

#include <cstdint>

namespace inf::synth {

inline float constexpr effect_shp_min_gain = 0.1f;
inline float constexpr effect_shp_max_gain = 32.0f;
inline float constexpr effect_reverb_gain = 0.015f;
inline float constexpr effect_reverb_dry_scale = 2.0f;
inline float constexpr effect_reverb_wet_scale = 3.0f;
inline float constexpr effect_reverb_damp_scale = 0.4f;
inline float constexpr effect_reverb_room_scale = 0.28f;
inline float constexpr effect_reverb_room_offset = 0.7f;
inline float constexpr effect_dly_max_time_sec = 10.0f;
inline float constexpr effect_dly_min_time_sec = 0.001f;
inline float constexpr effect_dly_multi_hold_max_time_sec = 3.0f;
inline float constexpr effect_dly_multi_hold_min_time_sec = 0.0f;
inline float constexpr effect_flt_comb_max_ms = 5.0f;
inline float constexpr effect_flt_stvar_min_freq = 20.0f;
inline float constexpr effect_flt_stvar_max_freq = 20000.0f;
inline float constexpr effect_reverb_spread = 23.0f / 44100.0f;

inline std::int32_t constexpr effect_dly_max_taps = 8;
inline std::int32_t constexpr effect_reverb_comb_count = 8;
inline std::int32_t constexpr effect_reverb_allpass_count = 4;
inline std::int32_t constexpr effect_shp_cheby_min_terms = 3;
inline std::int32_t constexpr effect_shp_cheby_max_terms = 32;

inline float constexpr effect_reverb_allpass_length[effect_reverb_allpass_count] = {
  556.0f / 44100.0f, 441.0f / 44100.0f, 341.0f / 44100.0f, 225.0f / 44100.0f };
inline float constexpr effect_reverb_comb_length[effect_reverb_comb_count] = {
  1116.0f / 44100.0f, 1188.0f / 44100.0f, 1277.0f / 44100.0f, 1356.0f / 44100.0f,
  1422.0f / 44100.0f, 1491.0f / 44100.0f, 1557.0f / 44100.0f, 1617.0f / 44100.0f };

} // namespace inf::synth
#endif // INF_SYNTH_EFFECT_CONFIG_HPP