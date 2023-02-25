#ifndef INF_SYNTH_OUTPUT_TOPOLOGY_HPP
#define INF_SYNTH_OUTPUT_TOPOLOGY_HPP

#include <inf.synth/synth/config.hpp>
#include <inf.base/topology/param_descriptor.hpp>
#include <inf.base/topology/part_ui_descriptor.hpp>

#include <vector>
#include <cstdint>

namespace inf::synth {

extern base::param_descriptor const output_params[];
struct output_param_t { enum value { clip, voices, drain, prev_cpu, high, high_cpu, count }; };
typedef output_param_t::value output_param;

inline float constexpr output_table_col_widths[output_param::count] = 
{ 0.12f, 0.19f, 0.12f, 0.15f, 0.21f, 0.21f };
inline base::part_table_descriptor const output_table = 
{ false, output_param::count, output_table_col_widths, nullptr }; 

struct usage_source_t { enum value { 
  voice, master, vaudio, gaudio, vcv, gcv, 
  env, osc, vlfo, glfo, veffect, geffect, count }; };
typedef usage_source_t::value usage_source;
inline std::int32_t const usage_source_counts[usage_source::count] =
{ 1, 1, 1, 1, 1, 1, venv_count, vosc_count, vlfo_count, glfo_count, veffect_count, geffect_count };

} // namespace inf::synth
#endif // INF_SYNTH_OUTPUT_TOPOLOGY_HPP  