#include <inf.plugin.infernal_synth/effect/topology.hpp>
#include <inf.plugin.infernal_synth/effect/processor.hpp>
#include <inf.plugin.infernal_synth/effect/processor_shaper.hpp>

using namespace inf::base;

namespace inf::plugin::infernal_synth {

// Chebyshev polynomials of the first kind.
static inline void
generate_cheby_terms(float in, float* t, std::int32_t n)
{
  t[0] = 1.0f;
  t[1] = in;
  for (std::int32_t o = 2; o <= n; o++)
    t[o] = 2.0f * in * t[o - 1] - t[o - 2];
}

void
effect_processor::process_shp(effect_process_input const& input, float* const* out)
{
  switch (_shaper_type)
  {
  case effect_shaper_type::cheby_one: process_shp_cheby_one(input, out); break;
  case effect_shaper_type::cheby_sum: process_shp_cheby_sum(input, out); break;
  case effect_shaper_type::tanh: process_shp_basic(input, out, [](float x) { return std::tanh(x); }); break;
  case effect_shaper_type::sine: process_shp_basic(input, out, [](float x) { return std::sin(pi32 * x); }); break;
  case effect_shaper_type::fold: process_shp_basic(input, out, [](float x) { return effect_shaper_fold(x); }); break;
  case effect_shaper_type::clip: process_shp_basic(input, out, [](float x) { return std::clamp(x, -1.0f, 1.0f); }); break;
  default: assert(false); break;
  }
}
 
// https://www.kvraudio.com/forum/viewtopic.php?t=70372
void
effect_processor::process_shp_cheby_one(effect_process_input const& input, float* const* out)
{
  float t[effect_shp_cheby_max_terms + 1];
  float const* mix = input.params[effect_param::shp_mix];
  float const* gain = input.params[effect_param::shp_gain];
  _state->oversampler.rearrange(stereo_channels, _shp_over_order);
  _state->oversampler.process(input.audio_in, out, input.sample_count,
    [&](std::int32_t c, std::int32_t s, float sample) {
      generate_cheby_terms(std::clamp(sample * gain[s], -1.0f, 1.0f), t, _shp_cheby_terms);
      return (1.0f - mix[s]) * sample + mix[s] * t[_shp_cheby_terms]; });
}

// https://www.kvraudio.com/forum/viewtopic.php?t=70372
void
effect_processor::process_shp_cheby_sum(effect_process_input const& input, float* const* out)
{
  float t[effect_shp_cheby_max_terms + 1];
  float const* mix = input.params[effect_param::shp_mix];
  float const* gain = input.params[effect_param::shp_gain];
  float const* dcy = input.params[effect_param::shp_cheby_sum_decay];
  _state->oversampler.rearrange(stereo_channels, _shp_over_order);
  _state->oversampler.process(input.audio_in, out, input.sample_count,
    [&](std::int32_t c, std::int32_t s, float sample) {
      float sum = 0.0f;
      float scale = 0.0f;
      float amount = 1.0f;
      generate_cheby_terms(std::clamp(sample * gain[s], -1.0f, 1.0f), t, _shp_cheby_terms);
      for (std::int32_t o = 3; o <= _shp_cheby_terms; o++)
      {
        scale += amount;
        sum += t[o] * amount; 
        amount = std::max(0.0f, amount - (1.0f - dcy[s]));
      }
      return (1.0f - mix[s]) * sample + mix[s] * sum / scale;
    });
}

} // namespace inf::plugin::infernal_synth 