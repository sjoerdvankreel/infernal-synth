#ifndef INF_BASE_UI_CONTROLS_GRAPH_PLOT_HPP
#define INF_BASE_UI_CONTROLS_GRAPH_PLOT_HPP

#include <inf.base/plugin/graph_processor.hpp>
#include <inf.base/plugin/plugin_controller.hpp>
#include <inf.base/topology/part_descriptor.hpp>
#include <juce_gui_basics/juce_gui_basics.h>

namespace inf::base::ui {

class inf_graph_plot : 
public juce::Component
{
  part_id const _part_id;
  std::int32_t const _graph_type;
  inf::base::plugin_controller* const _controller;
  std::unique_ptr<graph_processor> _processor = {};
  
public:
  graph_processor* processor();
  void paint(juce::Graphics& g) override;  
  inf_graph_plot(inf::base::plugin_controller* controller, part_id part_id, std::int32_t graph_type):
  juce::Component(), _part_id(part_id), _graph_type(graph_type), _controller(controller) {}
};

} // namespace inf::base::ui
#endif // INF_BASE_UI_CONTROLS_GRAPH_PLOT_HPP