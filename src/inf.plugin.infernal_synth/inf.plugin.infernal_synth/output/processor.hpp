#ifndef INF_PLUGIN_INFERNAL_SYNTH_OUTPUT_PROCESSOR_HPP
#define INF_PLUGIN_INFERNAL_SYNTH_OUTPUT_PROCESSOR_HPP

#include <inf.plugin.infernal_synth/output/state.hpp>
#include <inf.plugin.infernal_synth/synth/topology.hpp>
#include <inf.plugin.infernal_synth/shared/audio_part_processor.hpp>

#include <inf.base/plugin/state.hpp>
#include <inf.base/topology/topology_info.hpp>

namespace inf::synth {

// Cpu usage, clip and voice info.
class output_processor:
public audio_part_processor
{
public:
  output_processor() = default;
  output_processor(base::topology_info const* topology, float sample_rate);
  void process(base::block_input const& input, output_info const& info, base::block_output& output);
};

inline output_processor::
output_processor(base::topology_info const* topology, float sample_rate):
audio_part_processor(topology, { part_type::output, 0 }, sample_rate, -1) {}

} // namespace inf::synth
#endif // INF_PLUGIN_INFERNAL_SYNTH_OUTPUT_PROCESSOR_HPP