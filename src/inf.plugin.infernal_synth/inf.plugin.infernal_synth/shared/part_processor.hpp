#ifndef INF_PLUGIN_INFERNAL_SYNTH_SHARED_PART_PROCESSOR_HPP
#define INF_PLUGIN_INFERNAL_SYNTH_SHARED_PART_PROCESSOR_HPP

#include <inf.base/topology/topology_info.hpp>
#include <cstdint>

namespace inf::synth {

class part_processor
{
  base::part_id _id;
  float _sample_rate;
  base::topology_info const* _topology;

public:
  base::part_id id() const;
  float sample_rate() const;
  base::topology_info const* topology() const;

protected:
  part_processor() = default;
  part_processor(base::topology_info const* topology, base::part_id id, float sample_rate);
};

inline base::part_id 
part_processor::id() const 
{ return _id; }

inline base::topology_info const*
part_processor::topology() const 
{ return _topology; }

inline float 
part_processor::sample_rate() const 
{ return _sample_rate; }

inline part_processor::
part_processor(base::topology_info const* topology, base::part_id id, float sample_rate):
_id(id), _sample_rate(sample_rate), _topology(topology) {}

} // namespace inf::synth
#endif // INF_PLUGIN_INFERNAL_SYNTH_SHARED_PART_PROCESSOR_HPP