#ifndef INF_PLUGIN_INFERNAL_SYNTH_LFO_PROCESSOR_BASIC_HPP
#define INF_PLUGIN_INFERNAL_SYNTH_LFO_PROCESSOR_BASIC_HPP

#include <inf.plugin.infernal_synth/lfo/config.hpp>
#include <inf.base/shared/support.hpp>

#include <cmath>
#include <cstdint>

namespace inf::synth {

struct lfo_basic_processor
{
  float const offset;
  float offset_phase(float phase) const;
};

struct lfo_pulse_processor : lfo_basic_processor
{
  float const pw;
  float operator()(float frequency, float phase) const;
}; 

struct lfo_saw_processor : lfo_basic_processor
{ float operator()(float frequency, float phase) const; };

struct lfo_triangle_processor : lfo_basic_processor
{ float operator()(float frequency, float phase) const; };

struct lfo_sine_processor : lfo_basic_processor
{ float operator()(float frequency, float phase) const; };

inline float
lfo_basic_processor::offset_phase(float phase) const
{
  float result = phase + offset;
  result -= std::floor(result);
  return result;
}

inline float 
lfo_saw_processor::operator()(float frequency, float phase) const
{ return offset_phase(phase); }

inline float 
lfo_triangle_processor::operator()(float frequency, float phase) const
{ return 1.0f - std::abs(2.0f * offset_phase(phase) - 1.0f); }

inline float 
lfo_sine_processor::operator()(float frequency, float phase) const
{ return 0.5f + 0.5f * std::sin(2.0f * base::pi32 * offset_phase(phase)); }

inline float 
lfo_pulse_processor::operator()(float frequency, float phase) const
{ 
  float const min_pw = 0.05f;
  return offset_phase(phase) < (min_pw + (1.0f - min_pw) * pw) * 0.5f ? 1.0f: 0.0f; 
}

} // namespace inf::synth
#endif // INF_PLUGIN_INFERNAL_SYNTH_LFO_PROCESSOR_BASIC_HPP