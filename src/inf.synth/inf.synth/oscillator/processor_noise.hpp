#ifndef INF_SYNTH_OSCILLATOR_PROCESSOR_NOISE_HPP
#define INF_SYNTH_OSCILLATOR_PROCESSOR_NOISE_HPP

#include <inf.base/shared/support.hpp>
#include <inf.synth/shared/support.hpp>
#include <inf.synth/oscillator/state.hpp>
#include <inf.synth/oscillator/config.hpp>

#include <cmath>
#include <cstdint>

namespace inf::synth {

// Note: unison works by drawing from the same random
// stream for each voice. The detune and offset parameters
// won't do anything, but stereo spreading still works.
struct osc_noise_processor
{
  oscillator_state* const state;
  std::int32_t const over_order;
  float const sample_rate;
  float const* const x_param;
  float const* const y_param;
  float const* const color_param;
  float const* const filter_param;

  float next_color_value(std::int32_t sample) const;
  float next_sample(std::int32_t voice, float frequency,
    float phase, float increment, std::int32_t sample) const;
  float operator()(std::int32_t voice, float frequency,
    float phase, float increment, std::int32_t sample) const;
};

// Produce next colored noise.
// Not 1/f^a, but still has controllable spectrum.
// Basically just stack N white noise streams, log2 spaced
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
osc_noise_processor::next_sample(std::int32_t voice, float frequency,
  float phase, float increment, std::int32_t sample) const
{
  float const offset = 0.01f;
  float const min_freq = 80.0f;
  float const max_freq = 20000.0f;

  float x = offset + (1.0f - offset) * x_param[sample];
  if (std::abs(phase - state->noise_prev_draw_phase) >= (1.0f / x - offset) * increment)
  {
    state->noise_prev_draw_phase = phase;
    float y = offset + (1.0f - offset) * y_param[sample];
    if (fast_rand_next(state->noise_rand_state_x) <= y)
      state->noise_prev_draw = next_color_value(sample);
  }

  // Prevent filter start from zero.
  if (!state->noise_started)
  {
    state->noise_started = true;
    state->noise_filter.in1 = state->noise_prev_draw;
    state->noise_filter.out1 = state->noise_prev_draw;
  }

  // Filter + pick either filtered or plain.
  state->noise_filter.init(sample_rate, min_freq + filter_param[sample] * (max_freq - min_freq));
  float filtered = state->noise_filter.next(state->noise_prev_draw);
  return filter_param[sample] < 1.0f? filtered: state->noise_prev_draw;
}

// Oversampling of the noise osc.
inline float
osc_noise_processor::operator()(std::int32_t voice, float frequency,
  float phase, float increment, std::int32_t sample) const
{
  float in = 0.0f;
  float out = 0.0f;
  float current = 0.0f;
  float* out_channel = &out;
  float const* in_channel = &in;
  state->noise_oversampler.process(&in_channel, &out_channel, 1, 
    [=, &current](std::int32_t _1, std::int32_t _2, std::int32_t over_s, float _3) {
      if(over_s == 0) 
        current = next_sample(voice, frequency, phase, increment, sample); 
      return current;
    });
  return out;
}

} // namespace inf::synth
#endif // INF_SYNTH_OSCILLATOR_PROCESSOR_NOISE_HPP