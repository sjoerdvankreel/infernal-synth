#ifndef INF_PLUGIN_INFERNAL_SYNTH_OSCILLATOR_PROCESSOR_KPS_HPP
#define INF_PLUGIN_INFERNAL_SYNTH_OSCILLATOR_PROCESSOR_KPS_HPP

#include <inf.base/shared/support.hpp>
#include <inf.plugin.infernal_synth/oscillator/state.hpp>
#include <inf.plugin.infernal_synth/oscillator/config.hpp>

#include <cmath>
#include <cstdint>

namespace inf::synth {

// https://blog.demofox.org/2016/06/16/synthesizing-a-pluked-string-sound-with-the-karplus-strong-algorithm/
// https://github.com/marcociccone/EKS-string-generator/blob/master/Extended%20Karplus%20Strong%20Algorithm.ipynb
struct osc_kps_processor
{
  oscillator_state* state;
  float sample_rate;
  float const* const stretch_param;
  float const* const feedback_param;

  float operator()(std::int32_t voice, float frequency,  
    float phase, float increment, std::int32_t sample) const;
};

inline float
osc_kps_processor::operator()(std::int32_t voice, float frequency,
  float phase, float increment, std::int32_t sample) const
{
  float const min_feedback = 0.9f;
  float stretch = stretch_param[sample] * 0.5f;
  std::int32_t this_kps_length = static_cast<std::int32_t>(sample_rate / frequency);
  this_kps_length = std::min(this_kps_length, state->max_kps_length);
  std::int32_t this_index = state->kps_positions[voice];
  std::int32_t next_index = (state->kps_positions[voice] + 1) % this_kps_length;
  float result = state->kps_lines[voice][this_index];
  state->kps_lines[voice][this_index] = (0.5f + stretch) * state->kps_lines[voice][this_index];
  state->kps_lines[voice][this_index] += (0.5f - stretch) * state->kps_lines[voice][next_index];
  state->kps_lines[voice][this_index] *= min_feedback + feedback_param[sample] * (1.0f - min_feedback);
  if (++state->kps_positions[voice] >= this_kps_length) state->kps_positions[voice] = 0;
  return base::sanity(result);
}

} // namespace inf::synth
#endif // INF_PLUGIN_INFERNAL_SYNTH_OSCILLATOR_PROCESSOR_KPS_HPP