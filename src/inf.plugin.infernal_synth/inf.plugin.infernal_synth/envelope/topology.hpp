#ifndef INF_PLUGIN_INFERNAL_SYNTH_ENVELOPE_TOPOLOGY_HPP
#define INF_PLUGIN_INFERNAL_SYNTH_ENVELOPE_TOPOLOGY_HPP

#include <inf.base/topology/param_descriptor.hpp>
#include <inf.base/topology/topology_info.hpp>

#include <vector>
#include <cstdint>

namespace inf::plugin::infernal_synth {

extern base::param_descriptor const envelope_params[];
extern std::vector<float> const envelope_timesig_values;

inline char const* env_graph_name_selector(
base::topology_info const*, base::param_value const*, base::part_id, std::int32_t)
{ return "Envelope"; }

struct envelope_param_t { enum value {
  on, type, mode, synced, bipolar, invert, delay_time, delay_sync, 
  attack1_time, attack1_sync, attack1_slope, attack_split_level, 
  attack2_time, attack2_sync, attack2_slope, hold_time, hold_sync, 
  decay1_time, decay1_sync, decay1_slope, decay_split_level, 
  decay2_time, decay2_sync, decay2_slope, sustain_level, 
  release1_time, release1_sync, release1_slope, release_split_level, 
  release2_time, release2_sync, release2_slope, count }; };
typedef envelope_param_t::value envelope_param;

struct envelope_mode_t { enum value { legato, retrig, multi }; };
struct envelope_type_t { enum value { sustain, release, follow, count }; };
typedef envelope_type_t::value envelope_type;
typedef envelope_mode_t::value envelope_mode;

} // namespace inf::plugin::infernal_synth
#endif // INF_PLUGIN_INFERNAL_SYNTH_ENVELOPE_TOPOLOGY_HPP
