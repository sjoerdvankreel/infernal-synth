#include <inf.synth/shared/support.hpp>
#include <inf.synth/voice_master/graph.hpp>
#include <inf.synth/voice_master/topology.hpp>

using namespace inf::base;

namespace inf::synth {

bool
amp_bal_graph::needs_repaint(std::int32_t runtime_param) const
{
  std::int32_t begin = topology()->param_start(id());
  return begin <= runtime_param && runtime_param < begin + vgamp_param::count;
}

std::int32_t 
amp_bal_graph::sample_count(param_value const* state, float sample_rate) const
{ 
  std::int32_t const graph_length = 100;
  return graph_length;
}

void
amp_bal_graph::process_dsp_core(block_input const& input, float* output, float sample_rate)
{
  automation_view automation = input.data.automation.rearrange_params(id());
  float bal = automation.block_real_transform(vgamp_param::bal);
  float gain = automation.block_real_transform(vgamp_param::gain);
  for (std::int32_t i = 0; i < input.data.sample_count; i++)
  {
    std::int32_t channel = i < input.data.sample_count / 2? 0: 1;
    output[i] = gain * 0.5f * stereo_balance(channel, bal);
  }
}

} // namespace inf::synth