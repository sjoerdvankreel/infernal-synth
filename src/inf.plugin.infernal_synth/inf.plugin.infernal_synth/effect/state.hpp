#ifndef INF_PLUGIN_INFERNAL_SYNTH_EFFECT_STATE_HPP
#define INF_PLUGIN_INFERNAL_SYNTH_EFFECT_STATE_HPP

#include <inf.plugin.infernal_synth/effect/config.hpp>
#include <inf.base/shared/support.hpp>
#include <inf.base/shared/ring_buffer.hpp>
#include <inf.base/shared/oversampler.hpp>

namespace inf::plugin::infernal_synth {

struct effect_process_input
{
  std::int32_t sample_count;
  std::int32_t new_midi;
  std::int32_t new_midi_start_pos;
  float const* const* params;
  float const* const* audio_in;
};

// New midi: switching stvar filter keyboard tracking in mono/legato mode.
struct effect_input
{
  std::int32_t new_midi;
  std::int32_t new_midi_start_pos;
  float const* const* audio_in;
  base::block_input_data const* block;
};

struct effect_state
{
  bool global;
  base::oversampler oversampler;

  double flt_stvar_kbd_track_base;
  double flt_stvar_ic1eq[base::stereo_channels];
  double flt_stvar_ic2eq[base::stereo_channels];
  std::int32_t dly_fdbk_length[base::stereo_channels];

  std::array<base::ring_buffer<float>, base::stereo_channels> comb_input;
  std::array<base::ring_buffer<float>, base::stereo_channels> comb_output;
  std::array<base::ring_buffer<float>, base::stereo_channels> delay_buffer;

  std::array<std::array<float, effect_reverb_comb_count>, base::stereo_channels> reverb_comb_filter;
  std::array<std::array<std::int32_t, effect_reverb_comb_count>, base::stereo_channels> reverb_comb_pos;
  std::array<std::array<std::vector<float>, effect_reverb_comb_count>, base::stereo_channels> reverb_comb;
  std::array<std::array<std::int32_t, effect_reverb_allpass_count>, base::stereo_channels> reverb_allpass_pos;
  std::array<std::array<std::vector<float>, effect_reverb_allpass_count>, base::stereo_channels> reverb_allpass;

  void reset();
  effect_state() = default;
  effect_state(bool global, float sample_rate, std::int32_t max_sample_count);
};

} // namespace inf::plugin::infernal_synth
#endif // INF_PLUGIN_INFERNAL_SYNTH_EFFECT_STATE_HPP
