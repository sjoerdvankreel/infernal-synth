#include <inf.base.ui/shared/support.hpp>
#include <cassert>
#include <algorithm>

using namespace inf::base;

namespace inf::base::ui 
{

float
get_text_font_height(plugin_controller const* controller, float min_height, float max_height)
{
  float const current_width = static_cast<float>(controller->editor_current_width());
  float const scale_min_width = static_cast<float>(controller->editor_font_scaling_min_width());
  float const scale_max_width = static_cast<float>(controller->editor_font_scaling_max_width());
  if(current_width <= scale_min_width) return min_height;
  if(current_width >= scale_max_width) return max_height;
  float const factor = (current_width - scale_min_width) / (scale_max_width - scale_min_width);
  float const result = min_height + factor * (max_height - min_height);
  return static_cast<float>(static_cast<std::int32_t>(result));
}

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