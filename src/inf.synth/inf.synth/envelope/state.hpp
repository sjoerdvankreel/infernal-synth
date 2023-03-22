#ifndef INF_SYNTH_ENVELOPE_STATE_HPP
#define INF_SYNTH_ENVELOPE_STATE_HPP

#include <inf.base/plugin/state.hpp>
#include <cstdint>

namespace inf::synth {

struct envelope_split
{
  double level;
  double range;
};

struct envelope_stage
{
  double value;
  double accumulator;
  double inv_slope_range;
  std::int32_t samples;

  double next();
};

struct envelope_split_stage
{
  envelope_split split;
  envelope_stage stage1;
  envelope_stage stage2;
};

struct stage_param_ids
{
  std::int32_t time;
  std::int32_t sync;
  std::int32_t slope;
};

struct envelope_output
{
  bool ended;
  std::int64_t own_time;
};

// Offset/retrig: for mono-mode retrig/multitrig.
struct envelope_input
{
  std::int32_t offset;
  std::int32_t retrig_pos;
  std::int32_t release_sample;
  base::block_input_data const* block;
};

inline double
envelope_stage::next()
{
  value *= accumulator;
  return (value - 1.0) * inv_slope_range;
}

} // namespace inf::synth
#endif // INF_SYNTH_ENVELOPE_STATE_HPP