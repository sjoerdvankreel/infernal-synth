#ifndef INF_SYNTH_VOICE_MASTER_GRAPH_HPP
#define INF_SYNTH_VOICE_MASTER_GRAPH_HPP

#include <inf.base/plugin/graph_processor.hpp>
#include <cstdint>

namespace inf::synth {

class amp_bal_graph:
public base::graph_processor
{
public:
  amp_bal_graph(base::topology_info const* topology, base::part_id id);

public:
  bool needs_repaint(std::int32_t runtime_param) const override;
  bool bipolar(base::param_value const* state) const override { return true; }
  std::int32_t sample_count(base::param_value const* state, float sample_rate) const override;
  void process_dsp_core(base::block_input const& input, float* output, float sample_rate) override;
  void dsp_to_plot(base::graph_plot_input const& input, std::vector<float>& plot) override { plot = *input.dsp_output; }
};

inline amp_bal_graph::
amp_bal_graph(base::topology_info const* topology, base::part_id id):
base::graph_processor(topology, id) {}

} // namespace inf::synth
#endif // INF_SYNTH_VOICE_MASTER_GRAPH_HPP