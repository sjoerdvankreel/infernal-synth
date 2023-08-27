#include <inf.plugin.infernal_synth/effect/topology.hpp>
#include <inf.plugin.infernal_synth/effect/processor.hpp>
#include <inf.plugin.infernal_synth/shared/scratch_space.hpp>

using namespace inf::base;

namespace inf::plugin::infernal_synth {

// https://github.com/sinshu/freeverb
void
effect_processor::process_reverb(effect_process_input const& input, float* const* out, scratch_space& scratch)
{
  float* damp = scratch.storage_f32.buffer(0);
  float* size = scratch.storage_f32.buffer(1);
  float* scratch_in = scratch.storage_f32.buffer(2);

  float const* apf = input.params[effect_param::reverb_apf];
  float const* mix = input.params[effect_param::reverb_mix];
  float const* spread = input.params[effect_param::reverb_spread];
  float const* damp_param = input.params[effect_param::reverb_damp];
  float const* size_param = input.params[effect_param::reverb_size];

  // Precompute channel independent stuff.
  for (std::int32_t s = 0; s < input.sample_count; s++)
  {
    damp[s] = (1.0f - damp_param[s]) * effect_reverb_damp_scale;
    size[s] = (size_param[s] * effect_reverb_room_scale) + effect_reverb_room_offset;
    scratch_in[s] = (input.audio_in[0][s] + input.audio_in[1][s]) * effect_reverb_gain;
  }

  // Apply reverb per channel.
  for(std::int32_t c = 0; c < stereo_channels; c++)
    for (std::int32_t s = 0; s < input.sample_count; s++)
    {
      out[c][s] = 0.0f; // Needed for graph.
      for (std::int32_t i = 0; i < effect_reverb_comb_count; i++)
      {
        std::int32_t pos = _state->reverb_comb_pos[c][i];
        float comb = _state->reverb_comb[c][i][pos];
        std::int32_t length = static_cast<std::int32_t>(_state->reverb_comb[c][i].size());
        _state->reverb_comb_filter[c][i] = (comb * (1.0f - damp[s])) + (_state->reverb_comb_filter[c][i] * damp[s]);
        _state->reverb_comb[c][i][pos] = scratch_in[s] + (_state->reverb_comb_filter[c][i] * size[s]);
        _state->reverb_comb_pos[c][i] = (pos + 1) % length;
        out[c][s] += comb;
      }

      for (std::int32_t i = 0; i < effect_reverb_allpass_count; i++)
      {
        float output = out[c][s];
        std::int32_t pos = _state->reverb_allpass_pos[c][i];
        float allpass = _state->reverb_allpass[c][i][pos];
        std::int32_t length = static_cast<std::int32_t>(_state->reverb_allpass[c][i].size());
        out[c][s] = -out[c][s] + allpass;
        _state->reverb_allpass[c][i][pos] = output + (allpass * apf[s] * 0.5f);
        _state->reverb_allpass_pos[c][i] = (pos + 1) % length;
      }
    }

  // Final output depends on L/R both.
  for (std::int32_t s = 0; s < input.sample_count; s++)
  {
    float wet = mix[s] * effect_reverb_wet_scale;
    float dry = (1.0f - mix[s]) * effect_reverb_dry_scale;
    float wet1 = wet * (spread[s] / 2.0f + 0.5f); 
    float wet2 = wet * ((1.0f - spread[s]) / 2.0f);
    float out_l = out[0][s] * wet1 + out[1][s] * wet2;
    float out_r = out[1][s] * wet1 + out[0][s] * wet2;
    out[0][s] = out_l + input.audio_in[0][s] * dry;
    out[1][s] = out_r + input.audio_in[1][s] * dry;
  }
}

} // namespace inf::plugin::infernal_synth 