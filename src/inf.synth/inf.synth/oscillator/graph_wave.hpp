#ifndef SVN_SYNTH_OSCILLATOR_GRAPH_WAVE_HPP
#define SVN_SYNTH_OSCILLATOR_GRAPH_WAVE_HPP

#include <inf.synth/synth/config.hpp>
#include <inf.synth/shared/config.hpp>
#include <inf.synth/shared/support.hpp>

#include <inf.base/shared/support.hpp>
#include <inf.base/shared/cont_storage.hpp>
#include <inf.base/processor/graph_processor.hpp>

#include <array>
#include <vector>

namespace inf::synth {

class oscillator_wave_graph :
public inf::base::graph_processor
{
  bool const _right;
  std::vector<float> _port;
  std::array<base::cont_storage<float>, vosc_count> _vosc_out;
  std::array<base::cont_storage<float>, vosc_count> _vosc_reset_pos;
  std::array<base::cont_storage<std::int32_t>, vosc_count> _vosc_reset;
  std::array<std::vector<float>, inf::base::stereo_channels> _audio_out;

public:
  oscillator_wave_graph(base::topology_info const* topology, base::part_id id, bool right);

public:
  bool needs_repaint(std::int32_t runtime_param) const override;
  bool dsp_to_plot(base::graph_plot_input const& input, std::vector<float>& plot) override;
  std::int32_t sample_count(base::param_value const* state, float sample_rate) const override;
  void process_dsp_core(base::block_input const& input, float* output, float sample_rate) override;
  base::param_value transform_param(std::int32_t rt_index, base::param_value value) const override;
};

inline oscillator_wave_graph::
oscillator_wave_graph(base::topology_info const* topology, base::part_id id, bool right) :
graph_processor(topology, id), _right(right), 
_port(), _vosc_out(), _vosc_reset_pos(), _vosc_reset(), _audio_out() {}

inline base::param_value oscillator_wave_graph::
transform_param(std::int32_t rt_index, base::param_value value) const
{ return graph_disable_modulation(topology(), rt_index, value); }

} // namespace inf::synth
#endif // SVN_SYNTH_OSCILLATOR_GRAPH_WAVE_HPP
