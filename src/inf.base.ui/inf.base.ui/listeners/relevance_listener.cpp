#include <inf.base.ui/shared/support.hpp>
#include <inf.base.ui/listeners/relevance_listener.hpp>

namespace inf::base::ui
{

void 
relevance_listener::controller_param_changed(inf::base::param_value ui_value)
{ 
  std::int32_t part_index = _controller->topology()->params[_param_index].part_index;
  if(_hide) _component->setVisible(_selector(part_index, ui_value.discrete));
  else _component->setEnabled(_selector(part_index, ui_value.discrete));
}

} // namespace inf::base::ui