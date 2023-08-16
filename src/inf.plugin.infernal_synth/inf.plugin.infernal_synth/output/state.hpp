#ifndef INF_PLUGIN_INFERNAL_SYNTH_OUTPUT_STATE_HPP
#define INF_PLUGIN_INFERNAL_SYNTH_OUTPUT_STATE_HPP

#include <inf.plugin.infernal_synth/synth/config.hpp>
#include <cstdint>

namespace inf::synth {

struct cpu_usage
{
  std::int64_t env;
  std::int64_t osc;
  std::int64_t vcv;
  std::int64_t gcv;
  std::int64_t vlfo;
  std::int64_t glfo;
  std::int64_t vamp;
  std::int64_t gamp;
  std::int64_t vaudio;
  std::int64_t gaudio;
  std::int64_t veffect;
  std::int64_t geffect;
  std::int64_t previous;
};

struct output_info
{
  bool clipped;
  bool drained;
  cpu_usage usage;
  std::int64_t start_time;
  std::int32_t voice_count;
};

} // namespace inf::synth
#endif // INF_PLUGIN_INFERNAL_SYNTH_OUTPUT_STATE_HPP