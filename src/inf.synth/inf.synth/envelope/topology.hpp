#ifndef INF_SYNTH_ENVELOPE_TOPOLOGY_HPP
#define INF_SYNTH_ENVELOPE_TOPOLOGY_HPP

#include <inf.base/topology/param_descriptor.hpp>
#include <inf.base/topology/part_ui_descriptor.hpp>

#include <vector>
#include <cstdint>

namespace inf::synth {

extern base::param_descriptor const envelope_params[];
extern std::vector<float> const envelope_timesig_values;

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

inline char const* 
envelope_item_info(std::int32_t index)
{ return index > 0? nullptr: "Amp"; }; 
std::vector<base::graph_descriptor> const envelope_graphs =
{ { -1, "Envelope", { 0, 1, 3, 3 } } };
std::vector<base::box_descriptor> const envelope_borders =
{ { 0, 0, 2, 1 }, { 2, 0, 3, 1 }, { 5, 0, 3, 1 }, { 3, 1, 5, 1 }, { 3, 2, 5, 1 }, { 3, 3, 5, 1 } };

} // namespace inf::synth
#endif // INF_SYNTH_ENVELOPE_TOPOLOGY_HPP
