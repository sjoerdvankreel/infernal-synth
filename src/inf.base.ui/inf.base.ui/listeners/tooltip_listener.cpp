#include <inf.base.ui/listeners/tooltip_listener.hpp>

using namespace juce;
using namespace inf::base;

namespace inf::base::ui 
{

void 
tooltip_listener::controller_param_changed(param_value ui_value)
{
  auto const& info = _controller->topology()->params[_param_index];
  _client->setTooltip(juce::String(info.runtime_name) + ": " + info.descriptor->data.format(false, ui_value));
}

} // namespace inf::base::ui