#include <inf.base.ui/listeners/last_edit_value_param_listener.hpp>

using namespace juce;
using namespace inf::base;

namespace inf::base::ui {

void 
last_edit_value_param_listener::any_controller_param_changed(std::int32_t index)
{
  auto value = _controller->ui_value_at_index(index);
  auto const& param_info = _controller->topology()->params[index];
  auto const& part_desc = *_controller->topology()->parts[param_info.part_index].descriptor;
  if (part_desc.kind != part_kind::input) return;
  _editor->setText(_controller->topology()->params[index].descriptor->data.format(false, value));
}

} // namespace inf::base::ui