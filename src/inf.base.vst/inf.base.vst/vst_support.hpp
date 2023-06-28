#ifndef INF_BASE_VST_VST_SUPPORT_HPP
#define INF_BASE_VST_VST_SUPPORT_HPP

#include <inf.base/topology/topology_info.hpp>
#include <pluginterfaces/base/ftypes.h>
#include <pluginterfaces/vst/vsttypes.h>

#include <cassert>
#include <algorithm>

namespace inf::base::vst {

inline std::string
from_vst_string(Steinberg::Vst::TChar const* str)
{
  std::vector<char> result;
  while(*str != '\0' && *str < 256)
    result.push_back(*str++);
  result.push_back('\0');
  return std::string(result.data());
}

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
  inf::base::param_info const& info, std::int32_t val)
{ 
  assert(info.descriptor->data.type != inf::base::param_type::real);
  std::int32_t min = info.descriptor->data.discrete.min;
  std::int32_t max = info.descriptor->data.discrete.effective_max(info.part_index);
  return static_cast<double>(val - min) / (max - min); 
}

inline std::int32_t 
vst_normalized_to_discrete(
  inf::base::param_info const& info, double val)
{ 
  assert(info.descriptor->data.type != inf::base::param_type::real);
  std::int32_t min = info.descriptor->data.discrete.min;
  std::int32_t max = info.descriptor->data.discrete.effective_max(info.part_index);
  return min + std::clamp(static_cast<std::int32_t>(val * (max - min + 1)), 0, max - min);
}

inline double 
base_to_vst_normalized(
  inf::base::topology_info const* topology, std::int32_t param, inf::base::param_value val)
{
  auto const& info = topology->params[param];
  if (info.descriptor->data.type == inf::base::param_type::real) return val.real;
  return discrete_to_vst_normalized(info, val.discrete);
}

inline inf::base::param_value
vst_normalized_to_base(
  inf::base::topology_info const* topology, std::int32_t param, double val)
{ 
  auto const& info = topology->params[param];
  if(info.descriptor->data.type == inf::base::param_type::real) return inf::base::param_value(static_cast<float>(val));
  return inf::base::param_value(vst_normalized_to_discrete(info, val));
}

} // namespace inf::base::vst
#endif // INF_BASE_VST_VST_SUPPORT_HPP