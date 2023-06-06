#ifndef INF_BASE_UI_CONTROLS_GRAPH_PLOT_HPP
#define INF_BASE_UI_CONTROLS_GRAPH_PLOT_HPP

#include <inf.base/plugin/graph_processor.hpp>
#include <inf.base/plugin/plugin_controller.hpp>
#include <inf.base/topology/part_descriptor.hpp>

#include <juce_events/juce_events.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include <cstdint>

namespace inf::base::ui {

// Don't paint too often, is expensive.
class inf_graph_plot_timer:
public juce::Timer
{
  bool _dirty = false;
  bool _inside_callback = false;
  juce::Component* _plot = nullptr;
  std::uint64_t _paint_request_time = 0;
public:
  static inline std::uint64_t const timeout_millis = 50UL;
  void timerCallback() override;
  void delayed_repaint_request();
  void plot(juce::Component* plot) { _plot = plot; }
};

class inf_graph_plot : 
public juce::Component,
public juce::SettableTooltipClient
{
  part_id const _part_id;
  std::int32_t const _graph_type;
  inf_graph_plot_timer _repaint_timer = {};
  inf::base::plugin_controller* const _controller;
  std::unique_ptr<graph_processor> _processor = {};
  
public:
  graph_processor* processor();
  void paint(juce::Graphics& g) override;  
  void delayed_repaint_request() { _repaint_timer.delayed_repaint_request(); }

  ~inf_graph_plot();
  inf_graph_plot(inf::base::plugin_controller* controller, part_id part_id, std::int32_t graph_type);
};

} // namespace inf::base::ui
#endif // INF_BASE_UI_CONTROLS_GRAPH_PLOT_HPP