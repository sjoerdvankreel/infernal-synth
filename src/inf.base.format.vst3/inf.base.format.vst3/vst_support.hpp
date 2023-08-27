#ifndef INF_BASE_FORMAT_VST_VST_SUPPORT_HPP
#define INF_BASE_FORMAT_VST_VST_SUPPORT_HPP

#include <inf.base/topology/topology_info.hpp>
#include <pluginterfaces/base/ftypes.h>
#include <pluginterfaces/vst/vsttypes.h>

namespace inf::base::format::vst3 {

inline char constexpr hard_reset_request_msg_id[] = "XT_HARD_RESET_REQUEST";

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

} // namespace inf::base::format::vst3
#endif // INF_BASE_FORMAT_VST_VST_SUPPORT_HPP