#ifndef INF_BASE_UI_LISTENERS_GRAPH_LISTENER_HPP
#define INF_BASE_UI_LISTENERS_GRAPH_LISTENER_HPP

#include <inf.base.ui/controls/graph_plot.hpp>
#include <inf.base/plugin/plugin_controller.hpp>

namespace inf::base::ui {

class graph_listener :
public param_listener,
public any_param_listener
{
  inf_graph_plot* const _plot;
  base::part_id const _part_id;
  std::int32_t const _graph_type;
  std::int32_t const _tooltip_param;
  plugin_controller* const _controller;

public:
  void any_controller_param_changed(std::int32_t index) override;
  void controller_param_changed(inf::base::param_value ui_value) override;

  ~graph_listener();
  graph_listener(plugin_controller* controller, inf_graph_plot* plot, base::part_id part_id, std::int32_t graph_type, std::int32_t tooltip_param);
};

} // namespace inf::base::ui
#endif // INF_BASE_UI_LISTENERS_GRAPH_LISTENER_HPP