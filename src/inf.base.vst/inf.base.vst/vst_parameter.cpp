#include <inf.base.vst/vst_support.hpp>
#include <inf.base.vst/vst_parameter.hpp>
#include <cassert>

using namespace inf::base;
using namespace Steinberg;
using namespace Steinberg::Vst;

namespace inf::base::vst {
  
static double 
param_step_count(param_info const& info)
{
  switch (info.descriptor->data.type)
  {
  case param_type::real: return 0.0;
  default: return info.descriptor->data.discrete.effective_max(info.part_index) - info.descriptor->data.discrete.min;
  }
}

static double
param_default_to_vst_normalized(param_info const& info)
{
  switch (info.descriptor->data.type)
  {
  case param_type::real: return info.descriptor->data.real.default_;
  default: return discrete_to_vst_normalized(info, info.descriptor->data.discrete.default_);
  }
}

static int32
param_flags(param_descriptor const* param_desc)
{
  int32 result = 0;
  switch (param_desc->data.kind)
  {
  case param_kind::block: 
  case param_kind::voice:
  case param_kind::continuous:
    result = ParameterInfo::kCanAutomate; 
    break;
  case param_kind::output: 
    result = ParameterInfo::kIsReadOnly; 
    break;
  case param_kind::fixed: 
    result = ParameterInfo::kIsReadOnly | ParameterInfo::kIsHidden; 
    break;
  default: assert(false); break;
  }
  switch (param_desc->data.type)
  {  
  case param_type::list:
  case param_type::list_knob:
  case param_type::knob_list:
    result |= ParameterInfo::kIsList;
    break;
  default:
    break;
  }
  return result;
}     

vst_parameter::
vst_parameter(ParamID tag,
  part_info const* part, param_info const* info) :
  Parameter(
    to_vst_string(info->runtime_name.c_str()).c_str(),
    tag,
    to_vst_string(info->descriptor->data.unit).c_str(),
    param_default_to_vst_normalized(*info),
    param_step_count(*info),
    param_flags(info->descriptor),
    0,
    to_vst_string(info->descriptor->data.static_name.short_).c_str()),
  _info(info)
{    
  assert(info != nullptr);
  setPrecision(info->descriptor->data.real.precision);
}

ParamValue
vst_parameter::toPlain(ParamValue normalized) const
{
  switch (_info->descriptor->data.type)
  {
  case param_type::real: return _info->descriptor->data.real.display.to_range(normalized);
  default: return vst_normalized_to_discrete(*_info, normalized);
  }
}

ParamValue
vst_parameter::toNormalized(ParamValue plain) const
{
  switch (_info->descriptor->data.type)
  {
  case param_type::real: return _info->descriptor->data.real.display.from_range(plain);
  default: return discrete_to_vst_normalized(*_info, static_cast<std::int32_t>(plain));
  }
}

void
vst_parameter::toString(ParamValue normalized, String128 string) const
{
  param_value value;
  char str8[128] = { 0 };
  switch (_info->descriptor->data.type)
  {
  case param_type::real: value.real = toPlain(normalized); break;
  default: value.discrete = vst_normalized_to_discrete(*_info, normalized); break;
  }
  _info->descriptor->data.format(false, value, str8, 128);
  for(std::size_t i = 0; i < 128; i++)  
    string[i] = str8[i];
}
 
bool
vst_parameter::fromString(TChar const* string, ParamValue& normalized) const
{
  param_value value;
  std::vector<char> str8;
  while(*string != static_cast<TChar>(0))
    str8.push_back(static_cast<char>(*string++));
  str8.push_back('\0');
  if (!_info->descriptor->data.parse(false, _info->part_index, str8.data(), value)) return false;
  switch (_info->descriptor->data.type)
  {
  case param_type::real: normalized = toNormalized(value.real); break;
  default: normalized = discrete_to_vst_normalized(*_info, value.discrete); break;
  }
  return true;
}

} // namespace inf::base::vst