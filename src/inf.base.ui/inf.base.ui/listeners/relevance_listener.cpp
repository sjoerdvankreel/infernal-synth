#include <inf.base.ui/shared/support.hpp>
#include <inf.base.ui/listeners/relevance_listener.hpp>

namespace inf::base::ui
{

void 
relevance_listener::controller_param_changed(inf::base::param_value ui_value)
{ 
  if(_hide) _component->setVisible(_selector(ui_value.discrete));
  else _component->setEnabled(_selector(ui_value.discrete));
}

} // namespace inf::base::ui