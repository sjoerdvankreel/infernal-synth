#ifndef INF_BASE_UI_CONTROLS_GRAPH_PLOT_HPP
#define INF_BASE_UI_CONTROLS_GRAPH_PLOT_HPP

#include <inf.base/plugin/graph_processor.hpp>
#include <inf.base/plugin/plugin_controller.hpp>
#include <inf.base/topology/part_descriptor.hpp>

#include <juce_events/juce_events.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include <mutex>
#include <atomic>
#include <thread>
#include <cstdint>

namespace inf::base::ui {

class inf_graph_plot : 
public juce::Component,
public juce::SettableTooltipClient
{
  juce::Image _bg_image;
  part_id const _part_id;
  std::int32_t const _graph_type;
  std::string const _background_image_path;
  inf::base::plugin_controller* const _controller;
  std::unique_ptr<graph_processor> _processor = {};

  std::mutex _render_mutex;
  std::vector<base::graph_point> _graph_data = {};
  std::unique_ptr<std::thread> _render_thread = {};
  
  bool _bipolar_snapshot = {};
  float _opacity_snapshot = {};
  juce::Rectangle<float> _bounds_snapshot = {};
  std::vector<base::param_value> _state_snapshot = {};

  static void render_loop(inf_graph_plot* plot);

public:
  void repaint_request();
  graph_processor* processor();
  void paint(juce::Graphics& g) override;  

  inf_graph_plot(
    inf::base::plugin_controller* controller, part_id part_id, 
    std::int32_t graph_type, std::string const& background_image_path);
};

} // namespace inf::base::ui
#endif // INF_BASE_UI_CONTROLS_GRAPH_PLOT_HPP