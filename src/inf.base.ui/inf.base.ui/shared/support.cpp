#include <inf.base.ui/shared/support.hpp>
#include <cassert>

using namespace inf::base;

namespace inf::base::ui 
{

std::string
get_label_text(param_descriptor const* descriptor, label_kind kind, param_value value)
{
  switch (kind)
  {
  case label_kind::label: return descriptor->data.static_name.short_;
  case label_kind::value: return descriptor->data.format(false, value);
  case label_kind::both: return std::string(descriptor->data.static_name.short_) + " " + descriptor->data.format(false, value);
  default: assert(false); return {};
  }
}

} // namespace inf::base::ui