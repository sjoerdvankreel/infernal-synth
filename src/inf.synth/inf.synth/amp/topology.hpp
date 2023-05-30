#ifndef INF_SYNTH_AMP_TOPOLOGY_HPP
#define INF_SYNTH_AMP_TOPOLOGY_HPP

#include <inf.base/topology/param_descriptor.hpp>

namespace inf::synth {

struct amp_param_t { enum value { gain, bal, count }; };
typedef amp_param_t::value amp_param;
extern base::param_descriptor const vamp_params[];
extern base::param_descriptor const gamp_params[];

} // namespace inf::synth
#endif // INF_SYNTH_AMP_TOPOLOGY_HPP