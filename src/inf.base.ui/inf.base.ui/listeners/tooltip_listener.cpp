#include <inf.base.ui/listeners/tooltip_listener.hpp>

using namespace juce;
using namespace inf::base;

namespace inf::base::ui 
{

void 
tooltip_listener::controller_param_changed(param_value ui_value)
{
  auto const& desc = *_controller->topology()->params[_param_index].descriptor;
  _client->setTooltip(desc.data.format(false, ui_value) + desc.data.unit);    
}

} // namespace inf::base::ui