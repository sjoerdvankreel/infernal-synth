#include <inf.base.ui/listeners/graph_listener.hpp>

namespace inf::base::ui {

graph_listener::
~graph_listener() 
{
  _controller->remove_param_listener(_tooltip_param, this);
  _controller->remove_any_param_listener(this); 
}

graph_listener::
graph_listener(plugin_controller* controller, inf_graph_plot* plot, part_id part_id, std::int32_t graph_type, std::int32_t tooltip_param) :
_plot(plot), _part_id(part_id), _graph_type(graph_type), _tooltip_param(tooltip_param), _controller(controller)
{
  _controller->add_any_param_listener(this);
  _controller->add_param_listener(tooltip_param, this);
}

void 
graph_listener::any_controller_param_changed(std::int32_t index)
{ 
  if(_plot->processor()->needs_repaint(index)) 
    _plot->delayed_repaint_request();
}

void 
graph_listener::controller_param_changed(inf::base::param_value ui_value)
{
  auto const& desc = _controller->topology()->get_part_descriptor(_part_id);
  _plot->setTooltip(desc.name_selector(_controller->topology(), _controller->state(), _part_id, _graph_type));
}

} // namespace inf::base::ui