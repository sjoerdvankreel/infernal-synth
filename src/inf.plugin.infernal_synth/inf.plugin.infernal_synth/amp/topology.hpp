#ifndef INF_PLUGIN_INFERNAL_SYNTH_AMP_TOPOLOGY_HPP
#define INF_PLUGIN_INFERNAL_SYNTH_AMP_TOPOLOGY_HPP

#include <inf.base/topology/param_descriptor.hpp>

namespace inf::plugin::infernal_synth {

struct amp_param_t { enum value { gain, bal, count }; };
typedef amp_param_t::value amp_param;
extern base::param_descriptor const vamp_params[];
extern base::param_descriptor const gamp_params[];

} // namespace inf::plugin::infernal_synth
#endif // INF_PLUGIN_INFERNAL_SYNTH_AMP_TOPOLOGY_HPP