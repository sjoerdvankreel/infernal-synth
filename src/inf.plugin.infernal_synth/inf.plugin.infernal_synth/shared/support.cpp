#include <inf.plugin.infernal_synth/shared/support.hpp>
#include <inf.plugin.infernal_synth/cv_bank/topology.hpp>

using namespace inf::base;

namespace inf::synth {

param_value graph_disable_modulation(
topology_info const* topology, std::int32_t rt_index, param_value value)
{
  for (std::int32_t r = 0; r < cv_bank_route_count; r++)
    if (rt_index == topology->param_bounds[part_type::vcv_bank][0] + cv_bank_param_index(r, cv_bank_param_type::in))
      return param_value(0);
  for (std::int32_t r = 0; r < cv_bank_route_count; r++)
    if (rt_index == topology->param_bounds[part_type::gcv_bank][0] + cv_bank_param_index(r, cv_bank_param_type::in))
      return param_value(0);
  return value;
}

} // namespace inf::synth
