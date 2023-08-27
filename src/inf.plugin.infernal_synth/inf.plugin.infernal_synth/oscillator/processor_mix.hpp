#ifndef INF_PLUGIN_INFERNAL_SYNTH_OSCILLATOR_PROCESSOR_MIX_HPP
#define INF_PLUGIN_INFERNAL_SYNTH_OSCILLATOR_PROCESSOR_MIX_HPP

#include <inf.base/shared/support.hpp>
#include <inf.plugin.infernal_synth/oscillator/config.hpp>
#include <inf.plugin.infernal_synth/oscillator/processor_basic.hpp>

#include <cmath>
#include <cstdint>

namespace inf::plugin::infernal_synth {

struct osc_mix_processor
{
  float const* const sine_param;
  float const* const saw_param;
  float const* const pulse_param;
  float const* const triangle_param;
  float const* const mix_pw_param;

  float operator()(std::int32_t voice, float frequency,
    float phase, float increment, std::int32_t sample) const;
};

inline float
osc_mix_processor::operator()(std::int32_t voice, float frequency,
  float phase, float increment, std::int32_t sample) const
{
  float result = 0.0f;
  result += osc_sine_processor()(voice, frequency, phase, increment, sample) * sine_param[sample];
  result += osc_blep_saw_processor()(voice, frequency, phase, increment, sample) * saw_param[sample];
  result += osc_blamp_triangle_processor()(voice, frequency, phase, increment, sample) * triangle_param[sample];
  result += osc_blep_pulse_processor({ mix_pw_param })(voice, frequency, phase, increment, sample) * pulse_param[sample];
  return result;
}

} // namespace inf::plugin::infernal_synth
#endif // INF_PLUGIN_INFERNAL_SYNTH_OSCILLATOR_PROCESSOR_MIX_HPP