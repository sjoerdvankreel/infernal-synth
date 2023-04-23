#include <inf.base.ui/text_param_listener.hpp>

using namespace juce;
using namespace inf::base;

namespace inf::base::ui 
{

void 
text_param_listener::param_changed(param_value value)
{
  topology_info const* topology = _controller->topology();
  auto const desc = topology->params[_param_index].descriptor;
  std::string display_value = desc->data.format(false, value);
  _label->setText(display_value + desc->data.unit, dontSendNotification);
}

} // namespace inf::base::ui