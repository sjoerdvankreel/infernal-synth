#ifndef SVN_SYNTH_LFO_PROCESSOR_RANDOM_HPP
#define SVN_SYNTH_LFO_PROCESSOR_RANDOM_HPP

#include <svn.synth/lfo/state.hpp>
#include <svn.synth/lfo/config.hpp>
#include <svn.synth/lfo/topology.hpp>
#include <svn.base/shared/support.hpp>

#include <cmath>
#include <cstdint>

namespace svn::synth {

struct lfo_random_processor
{
  float const sample_rate;
  lfo_random_state* const state;

  float next_level() const;
  float next_slope() const;
  float operator()(float frequency, float phase) const;
}; 

// randomize level type
inline float 
lfo_random_processor::next_level() const
{
  switch (state->type)
  {
  case lfo_rand_type::slope: return state->level;
  case lfo_rand_type::level: return state->amt * fast_rand_next(state->state_y);
  case lfo_rand_type::both: return state->level + state->amt * fast_rand_next(state->state_y);
  default: assert(false); return 0.0f;
  }
}

// randomize slope on/off
inline float 
lfo_random_processor::next_slope() const
{
  switch (state->type)
  {
  case lfo_rand_type::level: return 0.0f;
  case lfo_rand_type::slope: case lfo_rand_type::both:
    return (fast_rand_next(state->state_y) - 0.5f) * state->amt * state->direction / static_cast<float>(state->length);
  default: assert(false); return 0.0f;
  }
}

inline float 
lfo_random_processor::operator()(float frequency, float phase) const
{
  // Finished segment, set new segment length/level/direction.
  // Last segment takes rounding errors.
  if (state->length == 0 && state->current_step < state->steps)
  {
    state->length = static_cast<std::int32_t>(std::ceil(sample_rate / frequency * state->distribution[state->current_step++]));
    assert(state->length > 0);
    state->level = next_level();
    state->slope = next_slope();
  }

  // Clamp and bounce.
  state->level += state->slope * state->amt * state->direction;
  state->level = std::clamp(state->level, -1.0f, 2.0f);
  if (state->level < 0.0f) state->level = -state->level, state->direction *= -1.0f;
  if (state->level > 1.0f) state->level = 1.0f - (state->level - 1.0f), state->direction *= -1.0f;
   state->length--;
  float result = base::sanity_unipolar(state->level);
  if (phase + frequency / sample_rate >= 1.0f) state->reset();
  return result;
}

} // namespace svn::synth
#endif // SVN_SYNTH_LFO_PROCESSOR_RANDOM_HPP