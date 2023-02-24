#ifndef INF_SYNTH_OUTPUT_STATE_HPP
#define INF_SYNTH_OUTPUT_STATE_HPP

#include <cstdint>
#include <inf.synth/synth/config.hpp>

namespace inf::synth {

struct cpu_usage
{
  std::int64_t aux;
  std::int64_t vcv;
  std::int64_t gcv;
  std::int64_t voice;
  std::int64_t vaudio;
  std::int64_t gaudio;
  std::int64_t master;
  std::int64_t previous;
  std::int64_t env[venv_count];
  std::int64_t osc[vosc_count];
  std::int64_t vlfo[vlfo_count];
  std::int64_t glfo[glfo_count];
  std::int64_t veffect[veffect_count];
  std::int64_t geffect[geffect_count];
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
#endif // INF_SYNTH_OUTPUT_STATE_HPP