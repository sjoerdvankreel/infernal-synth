#ifndef INF_PLUGIN_INFERNAL_SYNTH_EFFECT_GRAPH1_HPP
#define INF_PLUGIN_INFERNAL_SYNTH_EFFECT_GRAPH1_HPP

#include <inf.plugin.infernal_synth/effect/graph.hpp>
#include <inf.plugin.infernal_synth/shared/support.hpp>

#include <inf.base/shared/support.hpp>
#include <inf.base/plugin/graph_processor.hpp>

#include <array>
#include <vector>

namespace inf::synth {

class effect_graph1 :
public effect_graph_base
{
  float prepare_input_filter(float sample_rate); 
  float prepare_input_shaper(float sample_rate);
  float prepare_input_delay_reverb(base::block_input const& input, float rate);
  std::int32_t sample_count_delay(base::param_value const* state, float sample_rate) const;
  std::int32_t sample_count_filter(base::param_value const* state, float sample_rate) const;

public:
  effect_graph1(base::topology_info const* topology, base::part_id id);
  void process_dsp_lr(base::block_input const& input, float sample_rate, float* output_lr[2]);

  float opacity(base::param_value const* state) const override;
  bool needs_repaint(std::int32_t runtime_param) const override;
  bool bipolar(base::param_value const* state) const override { return true; }
  void dsp_to_plot(base::graph_plot_input const& input, std::vector<float>& plot) override;
  std::int32_t sample_count(base::param_value const* state, float sample_rate) const override;
  void process_dsp_core(base::block_input const& input, float* output, float sample_rate) override;
  base::param_value transform_param(std::int32_t rt_index, base::param_value value) const override;
};

inline effect_graph1::
effect_graph1(base::topology_info const* topology, base::part_id id) :
effect_graph_base(topology, id) {}

} // namespace inf::synth
#endif // INF_PLUGIN_INFERNAL_SYNTH_EFFECT_GRAPH1_HPP
