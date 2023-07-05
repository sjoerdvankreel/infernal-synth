#ifndef INF_SYNTH_OSCILLATOR_TOPOLOGY_HPP
#define INF_SYNTH_OSCILLATOR_TOPOLOGY_HPP

#include <inf.base/topology/param_descriptor.hpp>
#include <inf.base/topology/topology_info.hpp>

#include <vector>
#include <cstdint>

namespace inf::synth {

extern base::param_descriptor const osc_params[];
char const* osc_graph_name_selector(
base::topology_info const*, base::param_value const*, base::part_id, std::int32_t);

struct osc_graph_t { enum value { left, right, spectrum, count }; };
struct osc_type_t { enum value { basic, mix, dsf, kps, noise, count }; };
struct osc_basic_type_t { enum value { sine, saw, pulse, triangle, count }; };
struct osc_noise_type_t { enum value { sample_hold, linear, cubic_spline, count }; };
typedef osc_type_t::value osc_type;
typedef osc_graph_t::value osc_graph;
typedef osc_basic_type_t::value osc_basic_type;
typedef osc_noise_type_t::value osc_noise_type;

struct osc_param_t { enum value { 
  on, type, sync_src, kbd, oct, note, cent, pm, fm,
  am_src, am_ring, am_mix, uni_voices, uni_offset, uni_dtn, uni_sprd,
  basic_type, basic_pw, mix_sine, mix_saw, mix_triangle, mix_pulse, mix_pw,
  dsf_parts, dsf_dist, dsf_decay, kps_filter, kps_feedback, kps_stretch,
  noise_seed, noise_color, noise_x, noise_y, count }; };
typedef osc_param_t::value osc_param;
inline char const* osc_graph_names[osc_graph::count] = { "Left", "Right", "Spectrum" };

} // namespace inf::synth
#endif // INF_SYNTH_OSCILLATOR_TOPOLOGY_HPP
