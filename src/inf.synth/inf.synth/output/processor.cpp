#include <inf.base/shared/support.hpp>
#include <inf.synth/output/topology.hpp>
#include <inf.synth/output/processor.hpp>

using namespace inf::base;

namespace inf::synth {

void 
output_processor::process(base::block_input const& input, output_info const& info, base::block_output& output)
{
  double const nano = 1000.0 * 1000.0 * 1000.0;
  std::int32_t output_start = topology()->param_start(id());
  double total_time_factor = 100.0 * sample_rate() / (input.prev_sample_count * nano);

  // Clip and voice info.
  // Total time is measured from the previous processing round so we see the full cpu usage.
  output.block_automation_raw[output_start + output_param::voices].discrete = info.voice_count;
  output.block_automation_raw[output_start + output_param::clip].discrete = info.clipped ? 1 : 0;
  output.block_automation_raw[output_start + output_param::prev_cpu].discrete = static_cast<std::int32_t>(input.prev_perf_count * total_time_factor);
}

} // namespace inf::synth