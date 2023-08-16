#include <inf.base/shared/support.hpp>
#include <inf.plugin.infernal_synth/output/topology.hpp>
#include <inf.plugin.infernal_synth/output/processor.hpp>

using namespace inf::base;

namespace inf::synth {

void 
output_processor::process(base::block_input const& input, output_info const& info, base::block_output& output)
{ 
  double const nano = 1000.0 * 1000.0 * 1000.0;
  std::int32_t output_start = topology()->param_start(id());
  std::int64_t total_cpu = performance_counter() - info.start_time;
  double total_time_factor = 100.0 * sample_rate() / (input.prev_sample_count * nano);

  // Clip, voice and cpu info.
  // Total time is measured from the previous processing round so we see the full cpu usage.
  output.block_automation_raw[output_start + output_param::voices].discrete = info.voice_count;
  output.block_automation_raw[output_start + output_param::clip].discrete = info.clipped ? 1 : 0;
  output.block_automation_raw[output_start + output_param::drain].discrete = info.drained ? 1 : 0;
  output.block_automation_raw[output_start + output_param::prev_cpu].discrete = static_cast<std::int32_t>(input.prev_perf_count * total_time_factor);

  // Highest cpu usage module.
  std::int64_t highest_usage = 0;
  std::int32_t highest_use_mod = 0;
  if (info.usage.osc > highest_usage) highest_usage = info.usage.osc, highest_use_mod = usage_source::osc;
  if (info.usage.env > highest_usage) highest_usage = info.usage.env, highest_use_mod = usage_source::env;
  if (info.usage.vcv > highest_usage) highest_usage = info.usage.vcv, highest_use_mod = usage_source::vcv;
  if (info.usage.gcv > highest_usage) highest_usage = info.usage.gcv, highest_use_mod = usage_source::gcv;
  if (info.usage.vlfo > highest_usage) highest_usage = info.usage.vlfo, highest_use_mod = usage_source::vlfo;
  if (info.usage.glfo > highest_usage) highest_usage = info.usage.glfo, highest_use_mod = usage_source::glfo;
  if (info.usage.vamp > highest_usage) highest_usage = info.usage.vamp, highest_use_mod = usage_source::vamp;
  if (info.usage.gamp > highest_usage) highest_usage = info.usage.gamp, highest_use_mod = usage_source::gamp;
  if (info.usage.vaudio > highest_usage) highest_usage = info.usage.vaudio, highest_use_mod = usage_source::vaudio;
  if (info.usage.gaudio > highest_usage) highest_usage = info.usage.gaudio, highest_use_mod = usage_source::gaudio;
  if (info.usage.veffect > highest_usage) highest_usage = info.usage.veffect, highest_use_mod = usage_source::veffect;
  if (info.usage.geffect > highest_usage) highest_usage = info.usage.geffect, highest_use_mod = usage_source::geffect;
  std::int32_t high_use_cpu = static_cast<std::int32_t>(static_cast<double>(highest_usage) * 100 / total_cpu);
  output.block_automation_raw[output_start + output_param::high].discrete = highest_use_mod;
  output.block_automation_raw[output_start + output_param::high_cpu].discrete = high_use_cpu;
}

} // namespace inf::synth