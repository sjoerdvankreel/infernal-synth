#ifndef SVN_SYNTH_CV_BANK_GRAPH_HPP
#define SVN_SYNTH_CV_BANK_GRAPH_HPP

#include <inf.base/shared/state.hpp>
#include <inf.base/processor/graph_processor.hpp>

#include <vector>
#include <cstdint>

namespace inf::synth {

class cv_bank_graph:
public inf::base::graph_processor
{
public:
  cv_bank_graph(base::topology_info const* topology, base::part_id id);

  bool needs_repaint(std::int32_t runtime_param) const override;
  bool dsp_to_plot(base::graph_plot_input const& input, std::vector<float>& plot) override;
  std::int32_t sample_count(base::param_value const* state, float sample_rate) const override;
  void process_dsp_core(base::block_input const& input, float* output, float sample_rate) override;
};

inline bool
cv_bank_graph::needs_repaint(std::int32_t runtime_param) const
{ return true; }

inline cv_bank_graph::
cv_bank_graph(base::topology_info const* topology, base::part_id id) :
inf::base::graph_processor(topology, id) {}

} // namespace inf::synth
#endif // SVN_SYNTH_CV_BANK_GRAPH_HPP
