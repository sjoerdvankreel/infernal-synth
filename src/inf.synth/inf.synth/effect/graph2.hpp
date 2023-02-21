#ifndef SVN_SYNTH_EFFECT_GRAPH2_HPP
#define SVN_SYNTH_EFFECT_GRAPH2_HPP

#include <inf.synth/effect/graph1.hpp>
#include <inf.base/shared/support.hpp>
#include <inf.base/shared/spectrum_analyzer.hpp>
#include <inf.base/processor/graph_processor.hpp>

#include <array>
#include <vector>

namespace inf::synth {

class effect_graph2 :
public effect_graph_base
{
  base::fft _fft;
  effect_graph1 _graph1;
  inf::base::spectrum_analyzer _analyzer;

  void process_shaper_spectrum(base::block_input const& input, float* output, float sample_rate);

public:
  effect_graph2(base::topology_info const* topology, base::part_id id);

  float opacity(base::param_value const* state) const override;
  bool needs_repaint(std::int32_t runtime_param) const override;
  bool dsp_to_plot(base::graph_plot_input const& input, std::vector<float>& plot) override;
  std::int32_t sample_count(base::param_value const* state, float sample_rate) const override;
  base::param_value transform_param(std::int32_t rt_index, base::param_value value) const override;
  void process_dsp_core(base::block_input const& input, float* output, float sample_rate) override;
};

inline float 
effect_graph2::opacity(base::param_value const* state) const
{ return _graph1.opacity(state); }

inline bool
effect_graph2::needs_repaint(std::int32_t runtime_param) const
{ return _graph1.needs_repaint(runtime_param); }

inline base::param_value
effect_graph2::transform_param(std::int32_t rt_index, base::param_value value) const
{ return _graph1.transform_param(rt_index, value); }

inline effect_graph2::
effect_graph2(base::topology_info const* topology, base::part_id id) :
effect_graph_base(topology, id), _fft(), _graph1(topology, id), _analyzer() {}

} // namespace inf::synth
#endif // SVN_SYNTH_EFFECT_GRAPH2_HPP
