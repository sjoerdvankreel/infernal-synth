#ifndef INF_PLUGIN_INFERNAL_SYNTH_OSCILLATOR_PROCESSOR_NOISE_HPP
#define INF_PLUGIN_INFERNAL_SYNTH_OSCILLATOR_PROCESSOR_NOISE_HPP

#include <inf.base/shared/support.hpp>
#include <inf.plugin.infernal_synth/shared/support.hpp>
#include <inf.plugin.infernal_synth/oscillator/state.hpp>
#include <inf.plugin.infernal_synth/oscillator/config.hpp>

#include <cmath>
#include <cstdint>

namespace inf::plugin::infernal_synth {

// Note: unison works by drawing from the same random
// stream for each voice. The detune and offset parameters
// won't do anything, but stereo spreading still works.
struct osc_noise_processor
{
  oscillator_state* state;
  float const* const x_param;
  float const* const y_param;
  float const* const color_param;

  float next_color_value(std::int32_t sample) const;
  float operator()(std::int32_t voice, float frequency,
    float phase, float increment, std::int32_t sample) const;
};

// Produce next colored noise.
// Not really 1/f^a, but still has controllable spectrum.
// Basically we just stack N white noise streams, log2 spaced
// in frequency domain, with controllable rolloff per bin.
inline float 
osc_noise_processor::next_color_value(std::int32_t sample) const
{
  float limit = 0.5f;
  float result = 0.0f;
  float max_amp = 0.0f;
  float this_amp = 1.0f;
  float color = 1.0f - color_param[sample] * color_param[sample];

  for (std::int32_t i = 0; i < osc_noise_color_bin_count; i++)
  {
    if (state->noise_color_hold[i] == 0)
    {
      state->noise_color_hold[i] = 1 << i;
      state->noise_color_value[i] = fast_rand_next(state->noise_rand_state_y) * 2.0f - 1.0f;
    }
    state->noise_color_hold[i]--;
    result += state->noise_color_value[i] * this_amp;
    max_amp += this_amp / static_cast<float>(1 << i);
    this_amp *= color;
  }
  return result / max_amp * limit;
}

// Noise generator with x = how often to draw,
// y = do we really draw when x is met, color = spectrum control.
inline float
osc_noise_processor::operator()(std::int32_t voice, float frequency,
  float phase, float increment, std::int32_t sample) const
{
  float const offset = 0.01f;
  float x = offset + (1.0f - offset) * x_param[sample];
  if (std::abs(phase - state->noise_prev_draw_phase) >= (1.0f / x - offset) * increment)
  {
    state->noise_prev_draw_phase = phase;
    float y = offset + (1.0f - offset) * y_param[sample];
    if (fast_rand_next(state->noise_rand_state_x) <= y)
      state->noise_prev_draw = next_color_value(sample);
  }
  return state->noise_prev_draw;
}

} // namespace inf::plugin::infernal_synth
#endif // INF_PLUGIN_INFERNAL_SYNTH_OSCILLATOR_PROCESSOR_NOISE_HPP