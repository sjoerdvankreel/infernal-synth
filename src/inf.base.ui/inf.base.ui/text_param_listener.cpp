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
  _label->setText(desc->data.format(false, value), dontSendNotification);
}

} // namespace inf::base::ui