#include <inf.plugin.infernal_synth/shared/support.hpp>
#include <inf.plugin.infernal_synth/effect/topology.hpp>
#include <inf.plugin.infernal_synth/effect/processor.hpp>

using namespace inf::base;

namespace inf::plugin::infernal_synth {

void 
effect_processor::process_dly(effect_process_input const& input, float* const* out)
{
  switch (_delay_type)
  {
  case effect_delay_type::multi: process_dly_multi(input, out); break;
  case effect_delay_type::feedback: process_dly_feedback(input, out); break;
  default: assert(false); break;
  }
}

std::int32_t
effect_processor::graph_min_delay_samples() const
{
  assert(_state != nullptr);
  switch (_delay_type)
  {
  case effect_delay_type::multi: return _dly_multi_length;
  case effect_delay_type::feedback: return std::min(_state->dly_fdbk_length[0], _state->dly_fdbk_length[1]);
  default: assert(false); return 0;
  }
}

// https://www.musicdsp.org/en/latest/Effects/154-most-simple-and-smooth-feedback-delay.html
void
effect_processor::process_dly_feedback(effect_process_input const& input, float* const* out)
{
  assert(_state->global);

  // Don't go on forever.
  float const max_feedback = 0.99f;
  float const* mix = input.params[effect_param::dly_mix];
  float const* amt = input.params[effect_param::dly_amt];
  float const* sprd = input.params[effect_param::dly_fdbk_sprd];

  for (std::int32_t s = 0; s < input.sample_count; s++)
  { 
    std::int32_t fdbk_length_l = _state->dly_fdbk_length[0];
    std::int32_t fdbk_length_r = _state->dly_fdbk_length[1];
    float buffer_sample_l = _state->delay_buffer[0].get(fdbk_length_l + _dly_hold_length);
    float buffer_sample_r = _state->delay_buffer[1].get(fdbk_length_r + _dly_hold_length);
    float wet_l_base = amt[s] * max_feedback * buffer_sample_l;
    float wet_r_base = amt[s] * max_feedback * buffer_sample_r;
    float wet_l = wet_l_base + (1.0f - sprd[s]) * wet_r_base;
    float wet_r = wet_r_base + (1.0f - sprd[s]) * wet_l_base; 
    _state->delay_buffer[0].push(input.audio_in[0][s] + wet_l_base);
    _state->delay_buffer[1].push(input.audio_in[1][s] + wet_r_base);
    out[0][s] = (1.0f - mix[s]) * input.audio_in[0][s] + mix[s] * wet_l;
    out[1][s] = (1.0f - mix[s]) * input.audio_in[1][s] + mix[s] * wet_r;
  }
}

// https://www.musicdsp.org/en/latest/Effects/153-most-simple-static-delay.html
void
effect_processor::process_dly_multi(effect_process_input const& input, float* const* out)
{
  assert(_state->global);
  float const* mix = input.params[effect_param::dly_mix];
  float const* amt = input.params[effect_param::dly_amt];
  float const* sprd = input.params[effect_param::dly_multi_sprd];

  for (std::int32_t c = 0; c < stereo_channels; c++)
    for (std::int32_t s = 0; s < input.sample_count; s++)
    {
      float wet = 0.0f;
      float tap_amt = 1.0f - (1.0f - amt[s]) * (1.0f - amt[s]);
      for (std::int32_t t = 0; t < _dly_multi_taps; t++)
      {
        std::int32_t lr = (t + c) % 2;
        std::int32_t tap_length = (t + 1) * _dly_multi_length;
        float tap_bal = stereo_balance(lr, sprd[s]);
        float buffer_sample = _state->delay_buffer[c].get(tap_length + _dly_hold_length);
        wet += tap_bal * tap_amt * buffer_sample;
        tap_amt *= tap_amt;
      }
      _state->delay_buffer[c].push(input.audio_in[c][s]);
      out[c][s] = (1.0f - mix[s]) * input.audio_in[c][s] + (mix[s] * wet);
    }
}

} // namespace inf::plugin::infernal_synth 