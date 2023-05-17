#ifndef INF_BASE_UI_LISTENERS_GRAPH_LISTENER_HPP
#define INF_BASE_UI_LISTENERS_GRAPH_LISTENER_HPP

#include <inf.base.ui/controls/graph_plot.hpp>
#include <inf.base/plugin/plugin_controller.hpp>

namespace inf::base::ui {

class graph_listener :
public any_param_listener
{
  inf_graph_plot* const _plot;
  plugin_controller* const _controller;

public:
  void any_controller_param_changed(std::int32_t index) override;
  ~graph_listener() { _controller->remove_any_param_listener(this); }
  graph_listener(plugin_controller* controller, inf_graph_plot* plot):
  _plot(plot), _controller(controller) { _controller->add_any_param_listener(this); }
};

} // namespace inf::base::ui
#endif // INF_BASE_UI_LISTENERS_GRAPH_LISTENER_HPP