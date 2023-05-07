#ifndef INF_BASE_UI_LISTENERS_ENABLED_LISTENER_HPP
#define INF_BASE_UI_LISTENERS_ENABLED_LISTENER_HPP

#include <inf.base.ui/shared/support.hpp>
#include <inf.base/plugin/plugin_controller.hpp>

#include <juce_gui_basics/juce_gui_basics.h>

namespace inf::base::ui
{

typedef bool(*enabled_selector)(std::int32_t value);

class enabled_listener :
public param_listener
{
  std::int32_t const _param_index;
  enabled_selector const _selector;
  juce::Component* const _component;
  plugin_controller* const _controller;

public:
  void controller_param_changed(inf::base::param_value ui_value) override
  { _component->setEnabled(_selector(ui_value.discrete)); }
  ~enabled_listener() { _controller->remove_param_listener(_param_index, this); }
  enabled_listener(plugin_controller* controller, juce::Component* component, std::int32_t param_index, enabled_selector selector):
  _param_index(param_index), _selector(selector), _component(component), _controller(controller)
  { _controller->add_param_listener(param_index, this); }
};

} // namespace inf::base::ui
#endif // INF_BASE_UI_LISTENERS_ENABLED_LISTENER_HPP