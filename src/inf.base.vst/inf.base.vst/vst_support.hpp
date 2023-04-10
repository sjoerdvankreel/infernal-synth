#ifndef INF_BASE_VST_VST_SUPPORT_HPP
#define INF_BASE_VST_VST_SUPPORT_HPP

#include <inf.base/topology/topology_info.hpp>
#include <pluginterfaces/base/ftypes.h>
#include <pluginterfaces/vst/vsttypes.h>

#include <cassert>
#include <algorithm>

namespace inf::base::vst {

inline std::basic_string<Steinberg::Vst::TChar>
to_vst_string(char const* str)
{
  std::vector<Steinberg::Vst::TChar> result;
  while(*str != '\0')
    result.push_back(*str++);
  result.push_back(static_cast<Steinberg::Vst::TChar>('\0'));
  return std::basic_string<Steinberg::Vst::TChar>(result.data());
}

inline double 
discrete_to_vst_normalized(
  inf::base::param_descriptor const& param, std::int32_t val)
{ 
  assert(param.data.type != inf::base::param_type::real);
  std::int32_t min = param.data.discrete.min;
  std::int32_t max = param.data.discrete.max;
  return static_cast<double>(val - min) / (max - min); 
}

inline std::int32_t 
vst_normalized_to_discrete(
  inf::base::param_descriptor const& param, double val)
{ 
  assert(param.data.type != inf::base::param_type::real);
  std::int32_t min = param.data.discrete.min;
  std::int32_t max = param.data.discrete.max;
  return min + std::clamp(static_cast<std::int32_t>(val * (max - min + 1)), 0, max - min);
}

inline double 
base_to_vst_normalized(
  inf::base::topology_info const* topology, std::int32_t param, inf::base::param_value val)
{
  auto const& descriptor = *topology->params[param].descriptor;
  if (descriptor.data.type == inf::base::param_type::real) return val.real;
  return discrete_to_vst_normalized(descriptor, val.discrete);
}

inline inf::base::param_value
vst_normalized_to_base(
  inf::base::topology_info const* topology, std::int32_t param, double val)
{ 
  auto const& descriptor = *topology->params[param].descriptor;
  if(descriptor.data.type == inf::base::param_type::real) return inf::base::param_value(static_cast<float>(val));
  return inf::base::param_value(vst_normalized_to_discrete(descriptor, val));
}

} // namespace inf::base::vst
#endif // INF_BASE_VST_VST_SUPPORT_HPP