#include <inf.base/shared/support.hpp>
#include <inf.synth/output/topology.hpp>
#include <inf.synth/output/processor.hpp>

using namespace inf::base;

namespace inf::synth {

static std::vector<std::vector<std::int32_t>> const usage_table_in =
multi_list_table_init_in(usage_source_counts, usage_source::count);

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
  if (info.usage.vcv > highest_usage) 
  {
    highest_usage = info.usage.vcv;
    highest_use_mod = usage_table_in[usage_source::vcv][0];
  }
  if (info.usage.gcv > highest_usage)
  {
    highest_usage = info.usage.gcv;
    highest_use_mod = usage_table_in[usage_source::gcv][0];
  }
  if (info.usage.voice > highest_usage) 
  {
    highest_usage = info.usage.voice;
    highest_use_mod = usage_table_in[usage_source::voice][0];
  }
  if (info.usage.master > highest_usage) 
  {
    highest_usage = info.usage.master;
    highest_use_mod = usage_table_in[usage_source::master][0];
  }
  if (info.usage.vaudio > highest_usage)
  {
    highest_usage = info.usage.vaudio;
    highest_use_mod = usage_table_in[usage_source::vaudio][0];
  }
  if (info.usage.gaudio > highest_usage) 
  {
    highest_usage = info.usage.gaudio;
    highest_use_mod = usage_table_in[usage_source::gaudio][0];
  }
  for(std::int32_t i = 0; i < vosc_count; i++)
    if (info.usage.osc[i] > highest_usage) 
    {
      highest_usage = info.usage.osc[i]; 
      highest_use_mod = usage_table_in[usage_source::osc][i];
    }
  for (std::int32_t i = 0; i < venv_count; i++)
    if (info.usage.env[i] > highest_usage) 
    {
      highest_usage = info.usage.env[i];
      highest_use_mod = usage_table_in[usage_source::env][i];
    }
  for (std::int32_t i = 0; i < vlfo_count; i++)
    if (info.usage.vlfo[i] > highest_usage) 
    {
      highest_usage = info.usage.vlfo[i];
      highest_use_mod = usage_table_in[usage_source::vlfo][i];
    }
  for (std::int32_t i = 0; i < glfo_count; i++)
    if (info.usage.glfo[i] > highest_usage) 
    {
      highest_usage = info.usage.glfo[i];
      highest_use_mod = usage_table_in[usage_source::glfo][i];
    }
  for (std::int32_t i = 0; i < veffect_count; i++)
    if (info.usage.veffect[i] > highest_usage) 
    {
      highest_usage = info.usage.veffect[i];
      highest_use_mod = usage_table_in[usage_source::veffect][i];
    }
  for (std::int32_t i = 0; i < geffect_count; i++)
    if (info.usage.geffect[i] > highest_usage) 
    {
      highest_usage = info.usage.geffect[i];
      highest_use_mod = usage_table_in[usage_source::geffect][i];
    }
  std::int32_t high_use_cpu = static_cast<std::int32_t>(static_cast<double>(highest_usage) * 100 / total_cpu);
  output.block_automation_raw[output_start + output_param::high].discrete = highest_use_mod;
  output.block_automation_raw[output_start + output_param::high_cpu].discrete = high_use_cpu;
}

} // namespace inf::synth