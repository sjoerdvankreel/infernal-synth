#ifndef INF_BASE_UI_KNOB_LISTENER_HPP
#define INF_BASE_UI_KNOB_LISTENER_HPP

#include <juce_gui_basics/juce_gui_basics.h>
#include <inf.base/plugin/plugin_controller.hpp>

namespace inf::base::ui 
{

class knob_listener:
public juce::Slider::Listener,
public param_listener
{
  juce::Slider* const _slider;
  std::int32_t const _param_index;
  plugin_controller* const _controller;

public:
  void sliderValueChanged(juce::Slider* slider) override;
  void param_changed(inf::base::param_value value) override;

  ~knob_listener() { _controller->clear_param_listener(_param_index); }
  knob_listener(plugin_controller* controller, juce::Slider* slider, std::int32_t param_index):
  _slider(slider), _param_index(param_index), _controller(controller) { _controller->set_param_listener(param_index, this); }
};

} // namespace inf::base::ui
#endif // INF_BASE_UI_KNOB_LISTENER_HPP