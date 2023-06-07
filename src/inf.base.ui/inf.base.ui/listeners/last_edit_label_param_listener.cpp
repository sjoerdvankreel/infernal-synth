#include <inf.base.ui/listeners/last_edit_label_param_listener.hpp>

using namespace juce;
using namespace inf::base;

namespace inf::base::ui {

void 
last_edit_label_param_listener::any_controller_param_changed(std::int32_t index)
{
  auto const& param_info = _controller->topology()->params[index];
  auto const& part_desc = *_controller->topology()->parts[param_info.part_index].descriptor;
  if(part_desc.kind != part_kind::input || param_info.descriptor->data.kind == param_kind::ui) return;
  _label->setText(format_label_short(_controller, index), dontSendNotification);
}

} // namespace inf::base::ui