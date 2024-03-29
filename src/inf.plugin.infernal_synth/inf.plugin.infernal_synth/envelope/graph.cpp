#include <inf.plugin.infernal_synth/shared/state.hpp>
#include <inf.plugin.infernal_synth/shared/support.hpp>
#include <inf.plugin.infernal_synth/envelope/graph.hpp>
#include <inf.plugin.infernal_synth/envelope/topology.hpp>
#include <inf.plugin.infernal_synth/envelope/processor.hpp>

using namespace inf::base;

namespace inf::plugin::infernal_synth {

bool
envelope_graph::needs_repaint(std::int32_t runtime_param) const
{
  std::int32_t begin = topology()->param_start(id());
  return begin <= runtime_param && runtime_param < begin + envelope_param::count;
}

bool 
envelope_graph::bipolar(base::param_value const* state) const
{
  automation_view automation(topology(), state, id());
  return automation.block_discrete(envelope_param::bipolar) != 0;
}

void
envelope_graph::dsp_to_plot(graph_plot_input const& input, std::vector<float>& plot)
{
  plot.resize(input.dsp_output->size());
  automation_view automation(topology(), input.state, id());
  bool bipolar = automation.block_discrete(envelope_param::bipolar) != 0;
  for (std::size_t i = 0; i < input.dsp_output->size(); i++)
    plot[i] = bipolar ? ((*input.dsp_output)[i] + 1.0f) * 0.5f : (*input.dsp_output)[i];
}

std::int32_t
envelope_graph::sample_count(param_value const* state, float sample_rate) const
{
  automation_view automation(topology(), state, id());
  envelope_processor processor(topology(), id().index, cv_graph_rate, graph_bpm, automation);
  return static_cast<std::int32_t>(processor.total_dahdr_samples(true));
}

void
envelope_graph::process_dsp_core(block_input const& input, float* output, float sample_rate)
{
  cv_buffer buffer;
  buffer.values = output;

  envelope_input envelope_in;
  envelope_in.offset = 0;
  envelope_in.retrig_pos = -1;
  envelope_in.block = &input.data;

  envelope_processor processor(topology(), id().index, cv_graph_rate, graph_bpm, input.data.automation);
  envelope_in.release_sample = processor.total_dahdr_samples(false);
  std::memset(output, 0, input.data.sample_count * sizeof(float));
  processor.process(envelope_in, buffer);
}

} // namespace inf::plugin::infernal_synth