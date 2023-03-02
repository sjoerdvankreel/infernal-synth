#ifndef INF_SYNTH_SYNTH_TOPOLOGY_HPP
#define INF_SYNTH_SYNTH_TOPOLOGY_HPP

#include <inf.base/topology/part_descriptor.hpp>
#include <inf.base/processor/audio_processor.hpp>
#include <inf.base/processor/graph_processor.hpp>

#include <memory>
#include <cstdint>

namespace inf::synth {

extern base::part_descriptor const part_descriptors[];
  
// global topo
struct part_type_t { enum value { 
  vosc, veffect, geffect, vaudio_bank, gaudio_bank, 
  voice, master, venv, vlfo, glfo, vcv_bank, gcv_bank, 
  vcv_plot, gcv_plot, active, output, count }; };
typedef part_type_t::value part_type;

// plugin entry
struct synth_topology : 
public base::topology_info
{
public:
  std::unique_ptr<base::audio_processor> 
  create_audio_processor(
    base::param_value* state, std::int32_t* changed, 
    float sample_rate, std::int32_t max_sample_count) const override;
  std::unique_ptr<base::graph_processor>
  create_graph_processor(base::part_id id, std::int32_t graph_type) const override;
  base::param_value convert_param(
    std::int32_t index, base::param_value old_value,
    std::string const& old_text, std::uint16_t old_major, std::uint16_t old_minor) const override;

private:
  std::unique_ptr<base::graph_processor>
    create_graph_processor_effect(base::part_id id, std::int32_t graph_type) const;
  std::unique_ptr<base::graph_processor>
    create_graph_processor_oscillator(base::part_id id, std::int32_t graph_type) const;
};
 
} // namespace inf::synth
#endif // INF_SYNTH_SYNTH_TOPOLOGY_HPP