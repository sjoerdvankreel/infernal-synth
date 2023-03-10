#ifndef INF_SYNTH_LFO_STATE_HPP
#define INF_SYNTH_LFO_STATE_HPP

#include <inf.synth/lfo/config.hpp>
#include <inf.synth/shared/support.hpp>

#include <array>
#include <cstdint>

namespace inf::synth {

struct lfo_basic_state
{
  float pw;
  float offset;
  std::int32_t type;
};

struct lfo_custom_stage
{
  float factor;
  std::int32_t slope;
};

struct lfo_custom_state
{
  bool enabled;
  float hold1_factor;
  float hold2_factor;
  float delay1_factor;
  float delay2_factor;

  lfo_custom_stage rise1;
  lfo_custom_stage rise2;
  lfo_custom_stage fall1;
  lfo_custom_stage fall2;
};

struct lfo_random_state
{
  float amt;
  float level;
  float slope;
  float direction;

  std::int32_t type;
  std::int32_t steps;
  std::int32_t length;
  std::int32_t rand_steps;
  std::int32_t current_step;

  std::uint32_t state_x;
  std::uint32_t state_y;
  std::int32_t seed_x = 1;
  std::int32_t seed_y = 1;
  std::array<float, lfo_rand_max_steps> distribution;

  void reset();
};

inline void
lfo_random_state::reset()
{
  length = 0;
  slope = 0.0f;
  current_step = 0;
  direction = 1.0f;
  state_x = std::numeric_limits<uint32_t>::max() / seed_x;
  state_y = std::numeric_limits<uint32_t>::max() / seed_y;
  level = fast_rand_next(state_y);
}

} // namespace inf::synth
#endif // INF_SYNTH_LFO_STATE_HPP