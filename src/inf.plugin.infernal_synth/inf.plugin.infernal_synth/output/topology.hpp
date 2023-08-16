#ifndef INF_PLUGIN_INFERNAL_SYNTH_OUTPUT_TOPOLOGY_HPP
#define INF_PLUGIN_INFERNAL_SYNTH_OUTPUT_TOPOLOGY_HPP

#include <inf.plugin.infernal_synth/synth/config.hpp>
#include <inf.base/topology/param_descriptor.hpp>

#include <vector>
#include <cstdint>

namespace inf::plugin::infernal_synth {

struct usage_source_t { enum value { 
  vamp, gamp, vaudio, gaudio, vcv, gcv, 
  env, osc, vlfo, glfo, veffect, geffect, count }; };
typedef usage_source_t::value usage_source;

extern base::param_descriptor const output_params[];
struct output_param_t { enum value { clip, voices, drain, prev_cpu, high, high_cpu, count }; };
typedef output_param_t::value output_param;

} // namespace inf::plugin::infernal_synth
#endif // INF_PLUGIN_INFERNAL_SYNTH_OUTPUT_TOPOLOGY_HPP  