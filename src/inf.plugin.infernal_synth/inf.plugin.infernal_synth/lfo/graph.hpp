#ifndef INF_PLUGIN_INFERNAL_SYNTH_LFO_GRAPH_HPP
#define INF_PLUGIN_INFERNAL_SYNTH_LFO_GRAPH_HPP

#include <inf.base/plugin/graph_processor.hpp>
#include <cstdint>

namespace inf::plugin::infernal_synth {

class lfo_graph:
public inf::base::graph_processor
{
public:
  lfo_graph(base::topology_info const* topology, base::part_id id);

  bool bipolar(base::param_value const* state) const override;
  bool needs_repaint(std::int32_t runtime_param) const override;
  void dsp_to_plot(base::graph_plot_input const& input, std::vector<float>& plot) override;
  std::int32_t sample_count(base::param_value const* state, float sample_rate) const override;
  void process_dsp_core(base::block_input const& input, float* output, float sample_rate) override;
};

inline lfo_graph::
lfo_graph(base::topology_info const* topology, base::part_id id):
inf::base::graph_processor(topology, id) {}

} // namespace inf::plugin::infernal_synth
#endif // INF_PLUGIN_INFERNAL_SYNTH_LFO_GRAPH_HPP