#ifndef INF_SYNTH_OUTPUT_TOPOLOGY_HPP
#define INF_SYNTH_OUTPUT_TOPOLOGY_HPP

#include <inf.base/topology/param_descriptor.hpp>
#include <inf.base/topology/part_ui_descriptor.hpp>

#include <vector>
#include <cstdint>

namespace inf::synth {

extern base::param_descriptor const output_params[];
struct output_param_t { enum value { clip, voices, drain, prev_cpu, high, high_cpu, count }; };
typedef output_param_t::value output_param;

inline float constexpr output_table_col_widths[output_param::count] = 
{ 1.0 / 6.0f, 1.0 / 6.0f, 1.0 / 6.0f, 1.0 / 6.0f, 1.0 / 6.0f, 1.0 / 6.0f };
inline base::part_table_descriptor const output_table = 
{ false, output_param::count, output_table_col_widths, nullptr };

} // namespace inf::synth
#endif // INF_SYNTH_OUTPUT_TOPOLOGY_HPP  