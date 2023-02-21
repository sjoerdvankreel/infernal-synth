#ifndef SVN_SYNTH_OSCILLATOR_PROCESSOR_DSF_HPP
#define SVN_SYNTH_OSCILLATOR_PROCESSOR_DSF_HPP

#include <inf.base/shared/support.hpp>
#include <inf.synth/oscillator/config.hpp>

#include <cmath>
#include <cstdint>

namespace inf::synth {

// https://www.verklagekasper.de/synths/dsfsynthesis/dsfsynthesis.html
struct osc_dsf_processor
{
  float const sample_rate;
  std::int32_t const parts;
  float const* const dist_param;
  float const* const decay_param;

  float operator()(std::int32_t voice, float frequency, 
    float phase, float increment, std::int32_t sample) const;
};

inline float 
osc_dsf_processor::operator()(std::int32_t voice, float frequency,
  float phase, float increment, std::int32_t sample) const
{
  // -1: Fundamental is implicit. 
  std::int32_t ps = parts - 1;
  float const scale_factor = 0.975f;
  float const decay_range = 0.99f;
  float decay = decay_param[sample];
  float dist = frequency * dist_param[sample];
  float max_parts = (sample_rate * 0.5f - frequency) / dist;
  ps = std::min(ps, static_cast<std::int32_t>(max_parts));

  float n = static_cast<float>(ps);
  float w = decay * decay_range;
  float w_pow_np1 = std::pow(w, n + 1);
  float u = 2.0f * base::pi32 * phase;
  float v = 2.0f * base::pi32 * dist * phase / frequency;
  float a = w * std::sin(u + n * v) - std::sin(u + (n + 1) * v);
  float x = (w * std::sin(v - u) + std::sin(u)) + w_pow_np1 * a;
  float y = 1 + w * w - 2 * w * std::cos(v);
  float scale = (1.0f - w_pow_np1) / (1.0f - w);
  return base::sanity_bipolar(x * scale_factor / (y * scale));
}

} // namespace inf::synth
#endif // SVN_SYNTH_OSCILLATOR_PROCESSOR_DSF_HPP