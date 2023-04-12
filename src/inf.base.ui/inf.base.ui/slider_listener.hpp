#ifndef INF_BASE_UI_SLIDER_LISTENER_HPP
#define INF_BASE_UI_SLIDER_LISTENER_HPP

#include <juce_gui_basics/juce_gui_basics.h>
#include <inf.base/plugin/plugin_controller.hpp>

namespace inf::base::ui 
{

class slider_listener:
public juce::Slider::Listener
{
  std::int32_t const _param_index;
  plugin_controller* const _controller;

public:
  void sliderValueChanged(juce::Slider* slider);
  slider_listener(plugin_controller* controller, std::int32_t param_index):
  _controller(controller), _param_index(param_index) {}
};

} // namespace inf::base::ui
#endif // INF_BASE_UI_SLIDER_LISTENER_HPP