#ifndef SVN_VST_BASE_SDK_PARAMETER_HPP
#define SVN_VST_BASE_SDK_PARAMETER_HPP

#include <inf.base/topology/topology_info.hpp>
#include <public.sdk/source/vst/vstparameters.h>

#include <cstdint>
#include <algorithm>

namespace inf::vst::base {

// Vst parameter based on param_descriptor.
class vst_parameter:
public Steinberg::Vst::Parameter 
{
private:
  using TChar = Steinberg::Vst::TChar;
  using ParamID = Steinberg::Vst::ParamID;
  using String128 = Steinberg::Vst::String128;
  using ParamValue = Steinberg::Vst::ParamValue;
  inf::base::param_descriptor const* const _descriptor;

public:
  vst_parameter(ParamID tag,
    inf::base::part_info const* part,
    inf::base::param_info const* param);
  
  // Forwarding to param_descriptor.
  ParamValue toPlain(ParamValue normalized) const override;
  ParamValue toNormalized(ParamValue plain) const override;
  void toString(ParamValue normalized, String128 string) const override;
  bool fromString(TChar const* string, ParamValue& normalized) const override;
};

} // namespace inf::vst::base
#endif // SVN_VST_BASE_SDK_PARAMETER_HPP