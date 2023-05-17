#include <inf.base.ui/listeners/graph_listener.hpp>

namespace inf::base::ui {

void 
graph_listener::any_controller_param_changed(std::int32_t index)
{ 
  if(_plot->processor()->needs_repaint(index)) 
    _plot->repaint(); 
}

} // namespace inf::base::ui