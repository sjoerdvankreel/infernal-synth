#include <svn.synth/effect/topology.hpp>
#include <svn.synth/effect/processor.hpp>
#include <svn.synth/shared/scratch_space.hpp>

using namespace svn::base;

namespace svn::synth {

void 
effect_processor::update_flt_stvar_kbd_track(std::int32_t midi)
{
  _midi = midi;
  float this_frequency = note_to_frequency(_midi);
  float base_frequency = note_to_frequency(midi_note_c4);
  _state->flt_stvar_kbd_track_base = this_frequency / base_frequency;
}

void 
effect_processor::process_flt(effect_process_input const& input, float* const* out, scratch_space& scratch)
{
  switch (_filter_type)
  {
  case effect_filter_type::comb: process_flt_comb(input, out); break;
  case effect_filter_type::state_var: process_flt_stvar(input, out, scratch); break;
  default: assert(false); break;
  }
}

// https://www.dsprelated.com/freebooks/filters/Analysis_Digital_Comb_Filter.html
void
effect_processor::process_flt_comb(effect_process_input const& input, float* const* out)
{
  float const* gain_min = input.params[effect_param::flt_comb_gain_min];
  float const* gain_plus = input.params[effect_param::flt_comb_gain_plus];
  float const* dly_min_sec = input.params[effect_param::flt_comb_dly_min];
  float const* dly_plus_sec = input.params[effect_param::flt_comb_dly_plus];

  for(std::int32_t c = 0; c < stereo_channels; c++)
    for (std::int32_t s = 0; s < input.sample_count; s++)
    {
      std::int32_t dly_min_samples = static_cast<std::int32_t>(dly_min_sec[s] * sample_rate());
      std::int32_t dly_plus_samples = static_cast<std::int32_t>(dly_plus_sec[s] * sample_rate());
      float min = _state->comb_output[c].get(dly_min_samples) * gain_min[s];
      float plus = _state->comb_input[c].get(dly_plus_samples) * gain_plus[s];
      _state->comb_input[c].push(input.audio_in[c][s]);
      _state->comb_output[c].push(input.audio_in[c][s] + plus + min);
      out[c][s] = sanity(_state->comb_output[c].get(0));
    }
}

// https://cytomic.com/files/dsp/SvfLinearTrapOptimised2.pdf
void 
effect_processor::process_flt_stvar(effect_process_input const& input, float* const* out, scratch_space& scratch)
{
  // Precompute channel independent coefficients.
  double* g = scratch.storage_f64.buffer(0);
  double* k = scratch.storage_f64.buffer(1);
  double* a1 = scratch.storage_f64.buffer(2);
  double* shlf_a = scratch.storage_f64.buffer(3);
  double* shlf_k_mul = scratch.storage_f64.buffer(4);
  double* shlf_g_mul = scratch.storage_f64.buffer(5);

  float const* res = input.params[effect_param::flt_stvar_res];
  float const* kbd = input.params[effect_param::flt_stvar_kbd];
  float const* freq = input.params[effect_param::flt_stvar_freq];
  float const* gain = input.params[effect_param::flt_stvar_shlf_gain];

  // Per-sample gain parameters for shelving filters.
  switch (_flt_stvar_type)
  {
  case effect_flt_stvar_type::bll:
  case effect_flt_stvar_type::hsh:
  case effect_flt_stvar_type::lsh:
    for (std::int32_t s = 0; s < input.sample_count; s++)
      shlf_a[s] = std::pow(10.0, gain[s] / 40.0);
    break;
  default:
    // Gain is unused.
    break;
  }

  // Per-sample gain parameters for shelving filters.
  switch(_flt_stvar_type)
  {
  case effect_flt_stvar_type::bll:
    std::fill(shlf_g_mul, shlf_g_mul + input.sample_count, 1.0);
    for(std::int32_t s = 0; s < input.sample_count; s++)
      shlf_k_mul[s] = 1.0 / shlf_a[s];
    break;
  case effect_flt_stvar_type::hsh:
    std::fill(shlf_k_mul, shlf_k_mul + input.sample_count, 1.0);
    for (std::int32_t s = 0; s < input.sample_count; s++)
      shlf_g_mul[s] = std::sqrt(shlf_a[s]);
    break;
  case effect_flt_stvar_type::lsh:
    std::fill(shlf_k_mul, shlf_k_mul + input.sample_count, 1.0);
    for (std::int32_t s = 0; s < input.sample_count; s++)
      shlf_g_mul[s] = 1.0 / std::sqrt(shlf_a[s]);
    break;
  default:
    std::fill(shlf_g_mul, shlf_g_mul + input.sample_count, 1.0);
    std::fill(shlf_k_mul, shlf_k_mul + input.sample_count, 1.0);
    break;
  }

  for (std::int32_t s = 0; s < input.sample_count; s++)
  {
    // Midi switching for mono mode.
    if (s == input.new_midi_start_pos) update_flt_stvar_kbd_track(input.new_midi);
    double frequency = freq[s];
    if (kbd[s] > 0.0f) frequency = (1.0f - kbd[s]) * frequency + kbd[s] * frequency * _state->flt_stvar_kbd_track_base;
    if (kbd[s] < 0.0f) frequency = (1.0f + kbd[s]) * frequency - kbd[s] * frequency / _state->flt_stvar_kbd_track_base;
    frequency = std::clamp<double>(frequency, effect_flt_stvar_min_freq, effect_flt_stvar_max_freq);
    g[s] = std::tan(pi64 * frequency / sample_rate()) * shlf_g_mul[s];
    k[s] = (2.0 - 2.0 * res[s]) * shlf_k_mul[s];
    a1[s] = 1.0 / (1.0 + g[s] * (g[s] + k[s]));
  }

  // Run filter loop for each channel.
  for(std::int32_t c = 0; c < stereo_channels; c++)
    for (std::int32_t s = 0; s < input.sample_count; s++)
    {
      double a2 = g[s] * a1[s];
      double a3 = g[s] * a2;

      double v0 = input.audio_in[c][s];
      double v3 = v0 - _state->flt_stvar_ic2eq[c];
      double v1 = a1[s] * _state->flt_stvar_ic1eq[c] + a2 * v3;
      double v2 = _state->flt_stvar_ic2eq[c] + a2 * _state->flt_stvar_ic1eq[c] + a3 * v3;
      _state->flt_stvar_ic1eq[c] = 2.0 * v1 - _state->flt_stvar_ic1eq[c];
      _state->flt_stvar_ic2eq[c] = 2.0 * v2 - _state->flt_stvar_ic2eq[c];

      double sample = 0.0;
      switch (_flt_stvar_type)
      {
      case effect_flt_stvar_type::lpf: sample = v2; break;
      case effect_flt_stvar_type::bpf: sample = v1; break; 
      case effect_flt_stvar_type::bsf: sample = v0 - k[s] * v1; break;
      case effect_flt_stvar_type::hpf: sample = v0 - k[s] * v1 - v2; break;
      case effect_flt_stvar_type::apf: sample = v0 - 2.0 * k[s] * v1; break;
      case effect_flt_stvar_type::peq: sample = 2.0 * v2 - v0 + k[s] * v1; break;
      case effect_flt_stvar_type::bll: sample = v0 + (k[s] * (shlf_a[s] * shlf_a[s] - 1.0)) * v1; break;
      case effect_flt_stvar_type::lsh: sample = v0 + (k[s] * (shlf_a[s] - 1.0)) * v1 + (shlf_a[s] * shlf_a[s] - 1.0) * v2; break;
      case effect_flt_stvar_type::hsh: sample = shlf_a[s] * shlf_a[s] * v0 + (k[s] * (1.0 - shlf_a[s]) * shlf_a[s]) * v1 + (1.0 - shlf_a[s] * shlf_a[s]) * v2; break;
      default: assert(false); break;
      }  
      out[c][s] = sanity(static_cast<float>(sample));
    }
} 

} // namespace svn::synth 