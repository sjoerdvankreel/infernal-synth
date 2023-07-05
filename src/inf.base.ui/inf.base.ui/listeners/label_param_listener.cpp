#include <inf.base.ui/listeners/label_param_listener.hpp>

using namespace juce;
using namespace inf::base;

namespace inf::base::ui 
{

void 
label_param_listener::controller_param_changed(param_value ui_value)
{
  auto const desc = _controller->topology()->params[_param_index].descriptor;
  _label->setText(get_label_text(desc, _type, ui_value), dontSendNotification);
}

} // namespace inf::base::ui