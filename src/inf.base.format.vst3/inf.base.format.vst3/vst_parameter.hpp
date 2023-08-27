#ifndef INF_BASE_FORMAT_VST3_VST_PARAMETER_HPP
#define INF_BASE_FORMAT_VST3_VST_PARAMETER_HPP

#include <inf.base/shared/support.hpp>
#include <inf.base/topology/topology_info.hpp>
#include <public.sdk/source/vst/vstparameters.h>

#include <cstdint>
#include <algorithm>

namespace inf::base::format::vst3 {

// Vst parameter based on param_info.
class vst_parameter:
public Steinberg::Vst::Parameter 
{
private:
  using TChar = Steinberg::Vst::TChar;
  using ParamID = Steinberg::Vst::ParamID;
  using String128 = Steinberg::Vst::String128;
  using ParamValue = Steinberg::Vst::ParamValue;
  inf::base::param_info const* const _info;

public:
  vst_parameter(ParamID tag,
    inf::base::part_info const* part,
    inf::base::param_info const* info);
  
  // Forwarding to param_descriptor.
  void toString(ParamValue normalized, String128 string) const override;
  bool fromString(TChar const* string, ParamValue& normalized) const override;
  ParamValue toNormalized(ParamValue plain) const override { return display_to_format_normalized(*_info, true, plain); }
  ParamValue toPlain(ParamValue normalized) const override { return format_normalized_to_display(*_info, true, normalized); }
};

} // namespace inf::base::format::vst3
#endif // INF_BASE_FORMAT_VST3_VST_PARAMETER_HPP