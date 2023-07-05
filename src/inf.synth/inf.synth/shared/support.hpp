#ifndef INF_SYNTH_SHARED_SUPPORT_HPP
#define INF_SYNTH_SHARED_SUPPORT_HPP

#include <inf.base/shared/state.hpp>
#include <inf.synth/shared/state.hpp>
#include <inf.base/topology/topology_info.hpp>

#include <limits>
#include <cstdint>
#include <algorithm>

namespace inf::synth {

// For plotting.
inline float constexpr cv_graph_rate = 500.0f;
base::param_value graph_disable_modulation(
base::topology_info const* topology, std::int32_t rt_index, base::param_value value);

// 0 = right attenuated, 1 = left attenuated, 0.5 = no change.
// https://www.kvraudio.com/forum/viewtopic.php?t=148865.
inline float
stereo_balance(std::int32_t channel, float balance)
{
  assert(channel == 0 || channel == 1);
  if (channel == 0) return (1.0f - std::clamp(balance, 0.5f, 1.0f)) * 2.0f;
  if (channel == 1) return std::clamp(balance, 0.0f, 0.5f) * 2.0f;
  assert(false);
  return 0.0f;
}

// https://en.wikipedia.org/wiki/Lehmer_random_number_generator
inline float
fast_rand_next(std::uint32_t& state)
{
  float const max_int = static_cast<float>(std::numeric_limits<std::int32_t>::max());
  state = static_cast<std::uint64_t>(state) * 48271 % 0x7fffffff;
  return static_cast<float>(state) / max_int;
}

// From regular unipolar to inverted/bipolar/both.
inline void
unipolar_transform(cv_buffer& buffer, std::int32_t count)
{
  if(buffer.flags.inverted && !buffer.flags.bipolar)
    for(std::int32_t i = 0; i < count; i++)
      buffer.values[i] = 1.0f - buffer.values[i];
  else if(buffer.flags.bipolar && !buffer.flags.inverted)
    for (std::int32_t i = 0; i < count; i++)
      buffer.values[i] = buffer.values[i] * 2.0f - 1.0f;
  else if(buffer.flags.bipolar && buffer.flags.inverted)
    for (std::int32_t i = 0; i < count; i++)
      buffer.values[i] = (1.0f - buffer.values[i]) * 2.0f - 1.0f;
}

// From inverted/bipolar/both to regular unipolar.
inline void
unipolar_untransform(float* values, bool inverted, bool bipolar, std::int32_t count)
{
  if (inverted && !bipolar)
    for (std::int32_t i = 0; i < count; i++)
      values[i] = 1.0f - values[i];
  else if (bipolar && !inverted)
    for (std::int32_t i = 0; i < count; i++)
      values[i] = (values[i] + 1.0f) * 0.5f;
  else if (bipolar && inverted)
    for (std::int32_t i = 0; i < count; i++)
      values[i] = 1.0f - (values[i] + 1.0f) * 0.5f;
}

// From inverted/bipolar/both to regular unipolar.
inline void
unipolar_untransform(cv_buffer& buffer, std::int32_t count)
{ unipolar_untransform(buffer.values, buffer.flags.inverted, buffer.flags.bipolar, count); }

} // namespace inf::synth
#endif // INF_SYNTH_SHARED_SUPPORT_HPP