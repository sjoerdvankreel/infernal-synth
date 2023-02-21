#ifndef SVN_SYNTH_OSCILLATOR_TOPOLOGY_HPP
#define SVN_SYNTH_OSCILLATOR_TOPOLOGY_HPP

#include <inf.base/topology/param_descriptor.hpp>
#include <inf.base/topology/part_ui_descriptor.hpp>

#include <vector>
#include <cstdint>

namespace inf::synth {

extern base::param_descriptor const osc_params[];

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
  ram_src, ram_bal, ram_mix, uni_voices, uni_offset, uni_dtn, uni_sprd,
  basic_type, basic_pw, mix_sine, mix_saw, mix_triangle, mix_pulse, mix_pw,
  dsf_parts, dsf_dist, dsf_decay, kps_filter, kps_feedback, kps_stretch,
  noise_seed, noise_color, noise_x, noise_y, count }; };
typedef osc_param_t::value osc_param;

inline std::vector<base::box_descriptor> const osc_borders =
{ { 0, 0, 6, 1 }, { 6, 0, 3, 1 }, { 5, 1, 4, 1 }, { 5, 2, 4, 1 }, { 4, 1, 1, 2 } };
inline std::vector<base::graph_descriptor> const osc_graphs = { { osc_graph::left, "Left", { 0, 1, 2, 3 } },
{ osc_graph::right, "Right", { 2, 1, 2, 3 } }, { osc_graph::spectrum, "Spectrum", { 4, 3, 5, 1 } } };

} // namespace inf::synth
#endif // SVN_SYNTH_OSCILLATOR_TOPOLOGY_HPP
