#include <inf.synth/lfo/graph.hpp>
#include <inf.synth/lfo/topology.hpp>
#include <inf.synth/lfo/processor.hpp>
#include <inf.synth/shared/config.hpp>
#include <inf.synth/shared/scratch_space.hpp>

using namespace inf::base;

namespace inf::synth {

bool
lfo_graph::needs_repaint(std::int32_t runtime_param) const
{
  std::int32_t begin = topology()->param_start(id());
  return begin <= runtime_param && runtime_param < begin + lfo_param::count;
}

std::int32_t
lfo_graph::sample_count(param_value const* state, float sample_rate) const
{
  automation_view automation(topology(), state, id());
  float samples = cv_graph_rate / automation.block_real_transform(lfo_param::rate);
  if(automation.block_discrete(lfo_param::synced) != 0)
  {
    float timesig = lfo_timesig_values[automation.block_discrete(lfo_param::tempo)];
    samples = timesig_to_samples(cv_graph_rate, graph_bpm, timesig);
  }
  return static_cast<std::int32_t>(std::ceil(samples * cv_plot_cycles));
}

bool
lfo_graph::dsp_to_plot(graph_plot_input const& input, std::vector<float>& plot)
{
  plot.resize(input.dsp_output->size());
  automation_view automation(topology(), input.state, id());
  bool bipolar = automation.block_discrete(lfo_param::bipolar) != 0;
  for (std::size_t i = 0; i < input.dsp_output->size(); i++)
    plot[i] = bipolar ? ((*input.dsp_output)[i] + 1.0f) * 0.5f : (*input.dsp_output)[i];
  return bipolar;
}

void
lfo_graph::process_dsp_core(block_input const& input, float* output, float sample_rate)
{
  cv_buffer buffer;
  buffer.values = output;
  scratch_space scratch(input.data.sample_count);
  std::memset(output, 0, input.data.sample_count * sizeof(float));
  lfo_processor processor(topology(), id(), cv_graph_rate, input.data.bpm, input.data.automation);
  processor.process_any(input.data, buffer, scratch);
}

} // namespace inf::synth