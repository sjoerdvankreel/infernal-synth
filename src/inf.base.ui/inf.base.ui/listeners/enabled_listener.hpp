#ifndef INF_BASE_UI_LISTENERS_ENABLED_LISTENER_HPP
#define INF_BASE_UI_LISTENERS_ENABLED_LISTENER_HPP

#include <inf.base.ui/shared/support.hpp>
#include <inf.base/plugin/plugin_controller.hpp>

#include <juce_gui_basics/juce_gui_basics.h>

namespace inf::base::ui
{

class enabled_listener :
public param_listener
{
  std::int32_t const _param_index;
  std::int32_t const _param_value;
  juce::Component* const _component;
  plugin_controller* const _controller;

public:
  void controller_param_changed(inf::base::param_value ui_value) override
  { _component->setVisible(ui_value.discrete == _param_value); }
  ~enabled_listener() { _controller->remove_param_listener(_param_index, this); }
  enabled_listener(plugin_controller* controller, juce::Component* component, std::int32_t param_index, std::int32_t param_value):
  _param_index(param_index), _param_value(param_value), _component(component), _controller(controller) 
  { _controller->add_param_listener(param_index, this); }
};

} // namespace inf::base::ui
#endif // INF_BASE_UI_LISTENERS_ENABLED_LISTENER_HPP