#ifndef INF_PLUGIN_INFERNAL_SYNTH_VOICE_TOPOLOGY_HPP
#define INF_PLUGIN_INFERNAL_SYNTH_VOICE_TOPOLOGY_HPP

#include <inf.base/topology/param_descriptor.hpp>

#include <vector>
#include <cstdint>

namespace inf::synth {

struct voice_param_t { enum value { 
  oct, note, mode, port_mode, port_trig, 
  port_sync, port_time, port_tempo, count }; };
typedef voice_param_t::value voice_param;
extern base::param_descriptor const voice_params[];
extern std::vector<float> const voice_port_timesig_values;

struct voice_port_trig_t { enum value { note, voice }; };
struct voice_mode_t { enum value { poly, mono, release }; };
struct voice_port_mode_t { enum value { off, on, auto_ }; };
typedef voice_mode_t::value voice_mode;
typedef voice_port_mode_t::value voice_port_mode;
typedef voice_port_trig_t::value voice_port_trig;

} // namespace inf::synth
#endif // INF_PLUGIN_INFERNAL_SYNTH_VOICE_TOPOLOGY_HPP