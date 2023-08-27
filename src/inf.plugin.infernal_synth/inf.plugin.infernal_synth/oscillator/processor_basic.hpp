#ifndef INF_PLUGIN_INFERNAL_SYNTH_OSCILLATOR_PROCESSOR_BASIC_HPP
#define INF_PLUGIN_INFERNAL_SYNTH_OSCILLATOR_PROCESSOR_BASIC_HPP

#include <inf.base/shared/support.hpp>
#include <inf.plugin.infernal_synth/oscillator/config.hpp>

#include <cmath>
#include <cstdint>

namespace inf::plugin::infernal_synth {

struct osc_sine_processor
{
  float operator()(std::int32_t voice, float frequency, 
    float phase, float increment, std::int32_t sample) const;
};

// https://www.kvraudio.com/forum/viewtopic.php?t=375517
struct osc_blep_saw_processor
{
  float blep(float phase, float increment) const;
  float operator()(std::int32_t voice, float frequency, 
    float phase, float increment, std::int32_t sample) const;
};

struct osc_blep_pulse_processor
{  
  float const* const pw_param;
  float operator()(std::int32_t voice, float frequency, 
    float phase, float increment, std::int32_t sample) const;
};

// https://dsp.stackexchange.com/questions/54790/polyblamp-anti-aliasing-in-c
struct osc_blamp_triangle_processor
{
  float blamp(float phase, float increment) const;
  float operator()(std::int32_t voice, float frequency, 
    float phase, float increment, std::int32_t sample) const;
};

inline float 
osc_sine_processor::operator()(std::int32_t voice, float frequency,
  float phase, float increment, std::int32_t sample) const
{ return std::sin(2.0f * base::pi32 * phase); }

inline float 
osc_blep_saw_processor::blep(float phase, float increment) const
{
  float b;
  if (phase < increment) return b = phase / increment, (2.0f - b) * b - 1.0f;
  if (phase >= 1.0f - increment) return b = (phase - 1.0f) / increment, (b + 2.0f) * b + 1.0f;
  return 0.0f;
}

inline float 
osc_blep_saw_processor::operator()(std::int32_t voice, float frequency,
  float phase, float increment, std::int32_t sample) const
{
  phase += 0.5f;
  phase -= std::floor(phase);
  return base::sanity_bipolar((2.0f * phase - 1.0f) - blep(phase, increment));
}

inline float 
osc_blep_pulse_processor::operator()(std::int32_t voice, float frequency,
  float phase, float increment, std::int32_t sample) const
{
  osc_blep_saw_processor saw;
  float const min_pw = 0.05f;
  float pw = (min_pw + (1.0f - min_pw) * pw_param[sample]) * 0.5f;
  float saw1 = saw(voice, frequency, phase, increment, sample);
  float saw2 = saw(voice, frequency, phase + pw, increment, sample);
  return base::sanity_bipolar((saw1 - saw2) * 0.5f);
}

inline float 
osc_blamp_triangle_processor::blamp(float phase, float increment) const
{
  float y = 0.0f;
  if (!(0.0f <= phase && phase < 2.0f * increment)) 
    return y * increment / 15;
  float x = phase / increment;
  float u = 2.0f - x;
  u *= u * u * u * u;
  y -= u;
  if (phase >= increment) 
    return y * increment / 15;
  float v = 1.0f - x;
  v *= v * v * v * v;
  y += 4 * v;
  return y * increment / 15;
}

inline float 
osc_blamp_triangle_processor::operator()(std::int32_t voice,
  float frequency, float phase, float increment, std::int32_t sample) const
{
  float const triangle_scale = 0.9f;
  phase = phase + 0.75f;
  phase -= std::floor(phase);
  float result = 2.0f * std::abs(2.0f * phase - 1.0f) - 1.0f;
  result += blamp(phase, increment);
  result += blamp(1.0f - phase, increment);
  phase += 0.5f;
  phase -= std::floor(phase);
  result += blamp(phase, increment);
  result += blamp(1.0f - phase, increment);
  return base::sanity_bipolar(result * triangle_scale);
}

} // namespace inf::plugin::infernal_synth
#endif // INF_PLUGIN_INFERNAL_SYNTH_OSCILLATOR_PROCESSOR_BASIC_HPP