#ifndef INF_PLUGIN_INFERNAL_SYNTH_SHARED_AUDIO_PART_PROCESSOR_HPP
#define INF_PLUGIN_INFERNAL_SYNTH_SHARED_AUDIO_PART_PROCESSOR_HPP

#include <inf.plugin.infernal_synth/shared/part_processor.hpp>
#include <cstdint>

namespace inf::plugin::infernal_synth {

class audio_part_processor: 
public part_processor
{
  base::part_id _modulation_id;
public:
  base::part_id modulation_id() const;
protected:
  audio_part_processor() = default;
  audio_part_processor(base::topology_info const* topology, base::part_id id, float sample_rate, std::int32_t modulation_type);
};

inline base::part_id 
audio_part_processor::modulation_id() const 
{ return _modulation_id; }

inline audio_part_processor::
audio_part_processor(base::topology_info const* topology, base::part_id id, float sample_rate, std::int32_t modulation_type):
part_processor(topology, id, sample_rate), _modulation_id({ modulation_type, id.index }) {}

} // namespace inf::plugin::infernal_synth
#endif // INF_PLUGIN_INFERNAL_SYNTH_SHARED_AUDIO_PART_PROCESSOR_HPP