#ifndef INF_SYNTH_VOICE_MASTER_TOPOLOGY_HPP
#define INF_SYNTH_VOICE_MASTER_TOPOLOGY_HPP

#include <inf.base/topology/param_descriptor.hpp>

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

// ---- master ----
inline std::int32_t constexpr master_gcv_count = 3;
extern std::vector<float> const master_port_timesig_values;
extern base::param_descriptor const master_params[];

struct master_port_trig_t { enum value { note, voice }; };
struct master_mode_t { enum value { poly, mono, release }; };
struct master_port_mode_t { enum value { off, on, auto_ }; };
typedef master_mode_t::value master_mode;
typedef master_port_mode_t::value master_port_mode;
typedef master_port_trig_t::value master_port_trig;

struct master_param_t { enum value {
  gain, bal, mode, port_mode, port_trig, port_sync, port_time, port_tempo, 
  gcv1_uni, gcv1_bi, gcv2_uni, gcv2_bi, gcv3_uni, gcv3_bi, count }; };
typedef master_param_t::value master_param;

} // namespace inf::synth
#endif // INF_SYNTH_VOICE_MASTER_TOPOLOGY_HPP