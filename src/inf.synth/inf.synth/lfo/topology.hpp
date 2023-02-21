#ifndef SVN_SYNTH_LFO_TOPOLOGY_HPP
#define SVN_SYNTH_LFO_TOPOLOGY_HPP

#include <inf.base/topology/param_descriptor.hpp>
#include <inf.base/topology/part_ui_descriptor.hpp>

#include <vector>
#include <cstdint>

namespace inf::synth {

extern base::param_descriptor const vlfo_params[];
extern base::param_descriptor const glfo_params[];
extern std::vector<float> const lfo_timesig_values;

struct lfo_rand_type_t { enum value { slope, level, both }; };
struct lfo_type_t { enum value { basic, random, custom, count }; };
struct lfo_basic_type_t { enum value { sine, saw, pulse, triangle, count }; };
typedef lfo_type_t::value lfo_type;
typedef lfo_rand_type_t::value lfo_rand_type;
typedef lfo_basic_type_t::value lfo_basic_type;

struct lfo_param_t { enum value {
  on, type, bipolar, synced, invert, single, rate, tempo, filter,
  basic_type, basic_offset, basic_pw, rand_type, rand_seedx, rand_seedy, rand_rand_steps, rand_steps, rand_amt,
  custom_delay1, custom_rise1, custom_rise1_slope, custom_hold1, custom_fall1, custom_fall1_slope,
  custom_delay2, custom_fall2, custom_fall2_slope, custom_hold2, custom_rise2, custom_rise2_slope, count }; };
typedef lfo_param_t::value lfo_param;

inline std::vector<base::graph_descriptor> const lfo_graphs = { { -1, "LFO", { 0, 1, 3, 3 } } };
inline std::vector<base::box_descriptor> const lfo_borders = { { 0, 0, 3, 1 }, { 3, 0, 4, 1 }, { 3, 1, 4, 3 } };

} // namespace inf::synth
#endif // SVN_SYNTH_LFO_TOPOLOGY_HPP
