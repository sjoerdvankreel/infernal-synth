#ifndef INF_SYNTH_LFO_TOPOLOGY_HPP
#define INF_SYNTH_LFO_TOPOLOGY_HPP

#include <inf.base/topology/param_descriptor.hpp>
#include <inf.base/topology/topology_info.hpp>

#include <vector>
#include <cstdint>

namespace inf::synth {

extern base::param_descriptor const vlfo_params[];
extern base::param_descriptor const glfo_params[];
extern std::vector<float> const lfo_timesig_values;

inline char const* lfo_graph_name_selector(
base::topology_info const*, base::param_value const*, base::part_id, std::int32_t)
{ return "LFO"; }

struct lfo_rand_type_t { enum value { slope, level, both }; };
struct lfo_type_t { enum value { basic, random, free, count }; };
struct lfo_basic_type_t { enum value { sine, saw, pulse, triangle, count }; };
typedef lfo_type_t::value lfo_type;
typedef lfo_rand_type_t::value lfo_rand_type;
typedef lfo_basic_type_t::value lfo_basic_type;

struct lfo_param_t { enum value {
  on, type, bipolar, synced, invert, single, rate, tempo, filter,
  basic_type, basic_offset, basic_pw, rand_type, rand_free, rand_seedx, rand_seedy, rand_rand_steps, rand_steps, rand_amt,
  free_delay1, free_rise1, free_rise1_slope, free_hold1, free_fall1, free_fall1_slope,
  free_delay2, free_fall2, free_fall2_slope, free_hold2, free_rise2, free_rise2_slope, count }; };
typedef lfo_param_t::value lfo_param;

} // namespace inf::synth
#endif // INF_SYNTH_LFO_TOPOLOGY_HPP
