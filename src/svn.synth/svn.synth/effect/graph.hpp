#ifndef SVN_SYNTH_EFFECT_GRAPH_HPP
#define SVN_SYNTH_EFFECT_GRAPH_HPP

#include <svn.base/shared/support.hpp>
#include <svn.base/processor/graph_processor.hpp>

#include <array>
#include <vector>

namespace svn::synth {

class effect_graph_base:
public svn::base::graph_processor
{
protected:
  std::array<std::vector<float>, svn::base::stereo_channels> _audio_in;
  std::array<std::vector<float>, svn::base::stereo_channels> _audio_out;

  effect_graph_base(base::topology_info const* topology, base::part_id id);
  void process_graph(base::block_input const& input, float sample_rate);
};

inline effect_graph_base::
effect_graph_base(base::topology_info const* topology, base::part_id id):
svn::base::graph_processor(topology, id), _audio_in(), _audio_out() {}

} // namespace svn::synth
#endif // SVN_SYNTH_EFFECT_GRAPH_HPP