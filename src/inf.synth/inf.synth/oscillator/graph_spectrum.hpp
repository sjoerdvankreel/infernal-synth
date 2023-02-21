#ifndef INF_SYNTH_OSCILLATOR_GRAPH_SPECTRUM_HPP
#define INF_SYNTH_OSCILLATOR_GRAPH_SPECTRUM_HPP

#include <inf.synth/oscillator/graph_wave.hpp>

#include <inf.base/shared/support.hpp>
#include <inf.base/shared/spectrum_analyzer.hpp>
#include <inf.base/processor/graph_processor.hpp>

#include <array>
#include <vector>

namespace inf::synth {

class oscillator_spectrum_graph:
public inf::base::graph_processor
{
  oscillator_wave_graph _wave;
  inf::base::spectrum_analyzer _analyzer;

public:
  oscillator_spectrum_graph(base::topology_info const* topology, base::part_id id);

public:
  bool needs_repaint(std::int32_t runtime_param) const override;
  bool dsp_to_plot(base::graph_plot_input const& input, std::vector<float>& plot) override;
  std::int32_t sample_count(base::param_value const* state, float sample_rate) const override;
  void process_dsp_core(base::block_input const& input, float* output, float sample_rate) override;
  base::param_value transform_param(std::int32_t rt_index, base::param_value value) const override;
};

inline oscillator_spectrum_graph::
oscillator_spectrum_graph(base::topology_info const* topology, base::part_id id) :
graph_processor(topology, id), _wave(topology, id, false), _analyzer() {}

inline bool oscillator_spectrum_graph::
needs_repaint(std::int32_t runtime_param) const
{ return _wave.needs_repaint(runtime_param); }

inline base::param_value oscillator_spectrum_graph::
transform_param(std::int32_t rt_index, base::param_value value) const
{ return _wave.transform_param(rt_index, value); }

inline std::int32_t oscillator_spectrum_graph::
sample_count(base::param_value const* state, float sample_rate) const
{ return static_cast<std::int32_t>(std::ceil(sample_rate)); }

inline void oscillator_spectrum_graph::
process_dsp_core(base::block_input const& input, float* output, float sample_rate)
{ return _wave.process_dsp_core(input, output, sample_rate); }

} // namespace inf::synth
#endif // INF_SYNTH_OSCILLATOR_GRAPH_SPECTRUM_HPP
