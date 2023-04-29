#include <inf.base.ui/shared/support.hpp>
#include <cassert>

using namespace inf::base;

namespace inf::base::ui 
{

std::string
get_label_text(param_descriptor const* descriptor, label_type type, param_value value)
{
  switch (type)
  {
  case label_type::label: return descriptor->data.static_name.short_;
  case label_type::value: return descriptor->data.format(false, value);
  default: assert(false); return {};
  }
}

} // namespace inf::base::ui