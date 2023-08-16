#ifndef INF_PLUGIN_INFERNAL_SYNTH_SHARED_STATE_HPP
#define INF_PLUGIN_INFERNAL_SYNTH_SHARED_STATE_HPP

#include <cstdint>

namespace inf::plugin::infernal_synth {

// Cv attributes.
struct cv_flags
{
  bool bipolar;
  bool inverted;
};

// Cv block with per-block attributes.
struct cv_buffer
{
  float* values;
  cv_flags flags;
};

// Per-voice fixed values.
struct cv_hold_sample
{
  float value;
  cv_flags flags;
};

// Processing time and modulation time.
struct audio_part_output
{
  std::int64_t cv_time;
  std::int64_t own_time;
};

} // namespace inf::plugin::infernal_synth
#endif // INF_PLUGIN_INFERNAL_SYNTH_SHARED_STATE_HPP