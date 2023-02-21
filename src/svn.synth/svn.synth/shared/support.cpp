#include <svn.synth/shared/support.hpp>
#include <svn.synth/cv_bank/topology.hpp>

using namespace svn::base;

namespace svn::synth {

param_value graph_disable_modulation(
topology_info const* topology, std::int32_t rt_index, param_value value)
{
  for (std::int32_t b = 0; b < vcv_bank_count; b++)
    if (rt_index == topology->param_bounds[part_type::vcv_bank][b] + cv_bank_param::on)
      return param_value(0);
  for (std::int32_t b = 0; b < gcv_bank_count; b++)
    if (rt_index == topology->param_bounds[part_type::gcv_bank][b] + cv_bank_param::on)
      return param_value(0);
  return value;
}

} // namespace svn::synth
