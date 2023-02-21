#ifndef SVN_SYNTH_LFO_PROCESSOR_CUSTOM_HPP
#define SVN_SYNTH_LFO_PROCESSOR_CUSTOM_HPP

#include <inf.synth/lfo/config.hpp>
#include <inf.synth/lfo/state.hpp>
#include <inf.base/shared/support.hpp>

#include <cmath>
#include <cassert>
#include <cstdint>

namespace inf::synth {

struct lfo_custom_processor
{
  float const sample_rate;
  lfo_custom_state* const state;   
  float operator()(float frequency, float phase) const;
}; 

// X^0, X^1, X^2, X^4 ..., 1 - (1 - X^Y).
inline float 
lfo_custom_powi(float x, std::int32_t y)
{
  float z = base::sanity_unipolar(x);
  switch(y)  
  { 
  case 8: z *= z; [[fallthrough]];
  case 7: z *= z; [[fallthrough]];
  case 6: z *= z; [[fallthrough]];
  case 5: z *= z; [[fallthrough]];
  case 4: z *= z; [[fallthrough]];
  case 3: z *= z; [[fallthrough]];
  case 2: z *= z; [[fallthrough]];
  case 1: return z; case 0: return 1.0f;
  case -1: case -2: case -3: case -4:
  case -5: case -6: case -7: case -8: 
  return 1.0f - lfo_custom_powi(1.0f - x, -y);
  default: assert(false); return 0.0f;
  }
}

inline float
lfo_custom_slope(float phase, float pos, lfo_custom_stage stage)
{ return lfo_custom_powi((phase - pos) / stage.factor, stage.slope); }

inline float 
lfo_custom_processor::operator()(float frequency, float phase) const
{
  float pos = 0.0f;
  if (!state->enabled) return 0.0f;
  if (phase < pos + state->delay1_factor) return 0.5f;
  pos += state->delay1_factor;
  if (phase < pos + state->rise1.factor) return 0.5f + 0.5f * lfo_custom_slope(phase, pos, state->rise1);
  pos += state->rise1.factor;
  if (phase < pos + state->hold1_factor) return 1.0f;
  pos += state->hold1_factor;
  if (phase < pos + state->fall1.factor) return 0.5f + 0.5f * (1.0f - lfo_custom_slope(phase, pos, state->fall1));
  pos += state->fall1.factor;
  if (phase < pos + state->delay2_factor) return 0.5f;
  pos += state->delay2_factor;
  if (phase < pos + state->fall2.factor) return 0.5f - 0.5f * lfo_custom_slope(phase, pos, state->fall2);
  pos += state->fall2.factor;
  if (phase < pos + state->hold2_factor) return 0.0f;
  pos += state->hold2_factor;
  if (phase < pos + state->rise2.factor) return 0.5f * lfo_custom_slope(phase, pos, state->rise2);
  assert(phase < pos + state->rise2.factor + base::sanity_epsilon);
  return 0.0f;
}

} // namespace inf::synth
#endif // SVN_SYNTH_LFO_PROCESSOR_CUSTOM_HPP