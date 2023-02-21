#ifndef SVN_SYNTH_VOICE_MASTER_TOPOLOGY_HPP
#define SVN_SYNTH_VOICE_MASTER_TOPOLOGY_HPP

#include <inf.base/topology/param_descriptor.hpp>
#include <inf.base/topology/part_ui_descriptor.hpp>

#include <vector>
#include <cstdint>

namespace inf::synth {

// ---- shared ----
// voice and master params must start with these
struct vgamp_param_t { enum value { gain, bal, count }; };
typedef vgamp_param_t::value vgamp_param;

// ---- voice ----
struct voice_param_t { enum value { gain, bal, oct, note, count }; };
typedef voice_param_t::value voice_param;

extern base::param_descriptor const voice_params[];
inline std::vector<base::box_descriptor> const voice_borders = { { 0, 0, 2, 1 }, { 0, 1, 2, 1 } };
inline std::vector<base::graph_descriptor> const voice_graphs = { { -1, "Amp", { 0, 2, 2, 2 } } };

// ---- master ----
inline std::int32_t constexpr master_gcv_count = 3;
extern std::vector<float> const master_port_timesig_values;

extern base::param_descriptor const master_params[];
inline std::vector<base::graph_descriptor> const master_graphs = { { -1, "Master", { 0, 3, 3, 1 } } };
inline std::vector<base::box_descriptor> const master_borders = { { 0, 0, 4, 1 }, { 0, 1, 3, 2 }, { 3, 1, 1, 3 } };

struct master_port_trig_t { enum value { note, voice }; };
struct master_mode_t { enum value { poly, mono, release }; };
struct master_port_mode_t { enum value { off, on, auto_ }; };
typedef master_mode_t::value master_mode;
typedef master_port_mode_t::value master_port_mode;
typedef master_port_trig_t::value master_port_trig;

struct master_param_t { enum value {
  gain, bal, mode, port_mode, port_trig, port_sync, port_time, port_tempo, 
  gcv1, gcv1_bipolar, gcv2, gcv2_bipolar, gcv3, gcv3_bipolar, count }; };
typedef master_param_t::value master_param;

} // namespace inf::synth
#endif // SVN_SYNTH_VOICE_MASTER_TOPOLOGY_HPP