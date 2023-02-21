#include <inf.base/shared/support.hpp>
#include <inf.synth/output/topology.hpp>
#include <inf.synth/output/processor.hpp>

using namespace inf::base;

namespace inf::synth {

void 
output_processor::process(base::block_input const& input, output_info const& info, base::block_output& output)
{
  // Clip and voice info.
  std::int32_t output_start = topology()->param_start(id());
  output.block_automation_raw[output_start + output_param::voices].discrete = info.voice_count;
  output.block_automation_raw[output_start + output_param::clip].discrete = info.clipped ? 1 : 0;

  // Combined duration of processing parts.
  std::int64_t part_proc_time = 0;
  part_proc_time += info.usage.vcv + info.usage.gcv;
  part_proc_time += info.usage.vlfo + info.usage.glfo;
  part_proc_time += info.usage.vaudio + info.usage.gaudio;
  part_proc_time += info.usage.veffect + info.usage.geffect;
  part_proc_time += info.usage.osc + info.usage.env + info.usage.amp;

  double const nano = 1000.0 * 1000.0 * 1000.0;
  std::int64_t total_cpu = performance_counter() - info.start_time;
  double total_time_factor = 100.0 * sample_rate() / (input.prev_sample_count * nano);
  double part_time_factor = 100.0 / total_cpu;

  // Duration of individual processing parts.
  output.block_automation_raw[output_start + output_param::amp].discrete = static_cast<std::int32_t>(info.usage.amp * part_time_factor);
  output.block_automation_raw[output_start + output_param::osc].discrete = static_cast<std::int32_t>(info.usage.osc * part_time_factor);
  output.block_automation_raw[output_start + output_param::env].discrete = static_cast<std::int32_t>(info.usage.env * part_time_factor);
  output.block_automation_raw[output_start + output_param::vcv].discrete = static_cast<std::int32_t>(info.usage.vcv * part_time_factor);
  output.block_automation_raw[output_start + output_param::gcv].discrete = static_cast<std::int32_t>(info.usage.gcv * part_time_factor);
  output.block_automation_raw[output_start + output_param::vlfo].discrete = static_cast<std::int32_t>(info.usage.vlfo * part_time_factor);
  output.block_automation_raw[output_start + output_param::glfo].discrete = static_cast<std::int32_t>(info.usage.glfo * part_time_factor);
  output.block_automation_raw[output_start + output_param::vaudio].discrete = static_cast<std::int32_t>(info.usage.vaudio * part_time_factor);
  output.block_automation_raw[output_start + output_param::gaudio].discrete = static_cast<std::int32_t>(info.usage.gaudio * part_time_factor);
  output.block_automation_raw[output_start + output_param::veffect].discrete = static_cast<std::int32_t>(info.usage.veffect * part_time_factor);
  output.block_automation_raw[output_start + output_param::geffect].discrete = static_cast<std::int32_t>(info.usage.geffect * part_time_factor);

  // Aux is total this round minus combined processor usage.
  // Total time is measured from the previous processing round so we see the full cpu usage.
  output.block_automation_raw[output_start + output_param::prev_cpu].discrete = static_cast<std::int32_t>(input.prev_perf_count * total_time_factor);
  output.block_automation_raw[output_start + output_param::aux].discrete = static_cast<std::int32_t>((total_cpu - part_proc_time) * part_time_factor);
}

} // namespace inf::synth