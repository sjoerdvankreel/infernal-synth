#ifndef SVN_SYNTH_DSP_EFFECT_PROCESSOR_SHAPER_HPP
#define SVN_SYNTH_DSP_EFFECT_PROCESSOR_SHAPER_HPP

#include <inf.base/shared/support.hpp>
#include <inf.synth/effect/topology.hpp>
#include <inf.synth/effect/processor.hpp>

namespace inf::synth {

// Foldback shaper.
inline float
effect_shaper_fold(float x) 
{
  while (true)
    if(x > 1.0f) x -= 2.0f * (x - 1.0f);
    else if(x < -1.0f) x += 2.0f * (-x - 1.0f);
    else return x;
  assert(false);
  return 0.0f;
} 

template <class shaper_type>
void effect_processor::process_shp_basic(effect_process_input const& input, float* const* out, shaper_type shaper)
{
  float const* mix = input.params[effect_param::shp_mix];
  float const* gain = input.params[effect_param::shp_gain];
  _state->oversampler.rearrange(base::stereo_channels, _shp_over_order);
  _state->oversampler.process(input.audio_in, out, input.sample_count,
    [=](std::int32_t c, std::int32_t s, float sample) {
      return (1.0f - mix[s]) * sample + mix[s] * shaper(sample * gain[s]); });
}

} // namespace inf::synth
#endif // SVN_SYNTH_DSP_EFFECT_PROCESSOR_SHAPER_HPP