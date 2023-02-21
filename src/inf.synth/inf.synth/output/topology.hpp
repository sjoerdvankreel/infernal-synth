#ifndef INF_SYNTH_OUTPUT_TOPOLOGY_HPP
#define INF_SYNTH_OUTPUT_TOPOLOGY_HPP

#include <inf.base/topology/param_descriptor.hpp>
#include <inf.base/topology/part_ui_descriptor.hpp>

#include <vector>
#include <cstdint>

namespace inf::synth {

extern base::param_descriptor const output_params[];
inline std::int32_t constexpr output_table_col_count = 5;

struct output_param_t { enum value { 
  vlfo, glfo, vaudio, gaudio, veffect, geffect, vcv, gcv, 
  osc, env, amp, aux, clip, voices, prev_cpu, count }; };
typedef output_param_t::value output_param;

inline float constexpr output_table_col_widths[output_table_col_count] =
{ 0.2f, 0.2f, 0.2f, 0.2f, 0.2f };
inline base::part_table_descriptor const output_table = 
{ false, output_table_col_count, output_table_col_widths, nullptr };
inline std::vector<base::box_descriptor> const output_borders =
{ { 0, 0, 2, 1 }, { 0, 1, 2, 1 }, { 0, 2, 2, 1 }, { 0, 3, 2, 1 }, { 2, 0, 1, 4 }, { 0, 4, 3, 1 } };

} // namespace inf::synth
#endif // INF_SYNTH_OUTPUT_TOPOLOGY_HPP  