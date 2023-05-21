#ifndef INF_SYNTH_LFO_TOPOLOGY_HPP
#define INF_SYNTH_LFO_TOPOLOGY_HPP

#include <inf.base/topology/param_descriptor.hpp>

#include <vector>
#include <cstdint>

namespace inf::synth {

inline char const* lfo_graph_name = "Envelope";
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

} // namespace inf::synth
#endif // INF_SYNTH_LFO_TOPOLOGY_HPP
