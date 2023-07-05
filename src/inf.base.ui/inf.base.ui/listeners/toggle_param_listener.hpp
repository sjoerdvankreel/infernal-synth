#ifndef INF_BASE_UI_LISTENERS_TOGGLE_PARAM_LISTENER_HPP
#define INF_BASE_UI_LISTENERS_TOGGLE_PARAM_LISTENER_HPP

#include <juce_gui_basics/juce_gui_basics.h>
#include <inf.base/plugin/plugin_controller.hpp>

namespace inf::base::ui 
{

class toggle_param_listener :
public juce::Button::Listener,
public param_listener
{
  juce::Button* const _button;
  std::int32_t const _param_index;
  plugin_controller* const _controller;

public:
  void controller_param_changed(inf::base::param_value ui_value) override
  { _button->setToggleState(ui_value.discrete != 0, juce::dontSendNotification); }
  void buttonClicked(juce::Button* button) override
  { _controller->editor_param_changed(_param_index, param_value(button->getToggleState() ? 1 : 0)); }
  
  void buttonStateChanged(juce::Button*) override {}
  ~toggle_param_listener() { _controller->remove_param_listener(_param_index, this); }
  toggle_param_listener(plugin_controller* controller, juce::Button* button, std::int32_t param_index):
  _button(button), _param_index(param_index), _controller(controller) { _controller->add_param_listener(param_index, this); }
};

} // namespace inf::base::ui
#endif // INF_BASE_UI_LISTENERS_TOGGLE_PARAM_LISTENER_HPP