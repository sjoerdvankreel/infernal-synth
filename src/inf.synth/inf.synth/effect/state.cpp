#include <inf.synth/effect/state.hpp>
#include <inf.synth/effect/config.hpp>

namespace inf::synth {

effect_state::
effect_state(bool global, float sample_rate, std::int32_t max_sample_count):
global(global), oversampler(base::stereo_channels, max_sample_count), 
flt_stvar_kbd_track_base(), flt_stvar_ic1eq(), flt_stvar_ic2eq(), 
dly_fdbk_length(), comb_input(), comb_output(), delay_buffer(), reverb_comb_filter(), 
reverb_comb_pos(), reverb_comb(), reverb_allpass_pos(), reverb_allpass()
{
  float fmax_comb_length = std::ceil(sample_rate * effect_flt_comb_max_ms / 1000.0f);
  std::int32_t max_comb_length = static_cast<std::int32_t>(fmax_comb_length);
  for (std::int32_t c = 0; c < base::stereo_channels; c++)
  {
    comb_input[c] = base::ring_buffer<float>(max_comb_length);
    comb_output[c] = base::ring_buffer<float>(max_comb_length);
  }
  if (!global)
  {
    reset();
    return;
  }
  float fmax_delay_length = std::ceil(sample_rate * effect_dly_max_time_sec);
  std::int32_t max_delay_length = static_cast<std::int32_t>(fmax_delay_length);
  delay_buffer[0] = base::ring_buffer<float>(max_delay_length);
  delay_buffer[1] = base::ring_buffer<float>(max_delay_length);
  for (std::int32_t i = 0; i < effect_reverb_comb_count; i++)
  {
    float comb_length_l = effect_reverb_comb_length[i] * sample_rate;
    float comb_length_r = (effect_reverb_comb_length[i] + effect_reverb_spread) * sample_rate;
    reverb_comb[0][i] = std::vector<float>(static_cast<std::int32_t>(comb_length_l));
    reverb_comb[1][i] = std::vector<float>(static_cast<std::int32_t>(comb_length_r));
  }
  for (std::int32_t i = 0; i < effect_reverb_allpass_count; i++)
  {
    float apf_length_l = effect_reverb_allpass_length[i] * sample_rate;
    float apf_length_r = (effect_reverb_allpass_length[i] + effect_reverb_spread) * sample_rate;
    reverb_allpass[0][i] = std::vector<float>(static_cast<std::int32_t>(apf_length_l));
    reverb_allpass[1][i] = std::vector<float>(static_cast<std::int32_t>(apf_length_r));
  }
}

void 
effect_state::reset()
{
  for (std::int32_t c = 0; c < base::stereo_channels; c++)
  {
    comb_input[c].clear();
    comb_output[c].clear();
  }
  if (!global) return;
  for (std::int32_t c = 0; c < base::stereo_channels; c++)
  {
    delay_buffer[c].clear();
    for (std::int32_t i = 0; i < effect_reverb_comb_count; i++)
    {
      reverb_comb_pos[c][i] = 0;
      reverb_comb_filter[c][i] = 0.0f;
      std::fill(reverb_comb[c][i].begin(), reverb_comb[c][i].end(), 0.0f);
    }
    for (std::int32_t i = 0; i < effect_reverb_allpass_count; i++)
    {
      reverb_allpass_pos[c][i] = 0;
      std::fill(reverb_allpass[c][i].begin(), reverb_allpass[c][i].end(), 0.0f);
    }
  }
}

} // namespace inf::synth