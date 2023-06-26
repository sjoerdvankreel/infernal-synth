#ifndef INF_SYNTH_ENVELOPE_GRAPH_HPP
#define INF_SYNTH_ENVELOPE_GRAPH_HPP

#include <inf.base/plugin/graph_processor.hpp>
#include <cstdint>

namespace inf::synth {

class envelope_graph:
public inf::base::graph_processor
{
public:
  bool bipolar(base::param_value const* state) const override;
  bool needs_repaint(std::int32_t runtime_param) const override;
  void dsp_to_plot(base::graph_plot_input const& input, std::vector<float>& plot) override;
  std::int32_t sample_count(base::param_value const* state, float sample_rate) const override;
  void process_dsp_core(base::block_input const& input, float* output, float sample_rate) override;
  envelope_graph(base::topology_info const* topology, base::part_id id, std::vector<float>* raw_data_storage);
};

inline envelope_graph::
envelope_graph(base::topology_info const* topology, base::part_id id, std::vector<float>* raw_data_storage):
  inf::base::graph_processor(topology, id, raw_data_storage) {}

} // namespace inf::synth
#endif // INF_SYNTH_ENVELOPE_GRAPH_HPP
