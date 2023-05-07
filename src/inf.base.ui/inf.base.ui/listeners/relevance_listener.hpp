#ifndef INF_BASE_UI_LISTENERS_RELEVANCE_LISTENER_HPP
#define INF_BASE_UI_LISTENERS_RELEVANCE_LISTENER_HPP

#include <inf.base.ui/shared/support.hpp>
#include <inf.base/plugin/plugin_controller.hpp>

#include <juce_gui_basics/juce_gui_basics.h>

namespace inf::base::ui
{

typedef bool(*relevance_selector)(std::int32_t value);

class relevance_listener :
public param_listener
{
  bool const _hide;
  std::int32_t const _param_index;
  juce::Component* const _component;
  relevance_selector const _selector;
  plugin_controller* const _controller;

public:
  void controller_param_changed(inf::base::param_value ui_value) override;
  
  ~relevance_listener() { _controller->remove_param_listener(_param_index, this); }
  relevance_listener(plugin_controller* controller, juce::Component* component, std::int32_t param_index, bool hide, relevance_selector selector):
  _hide(hide), _param_index(param_index), _component(component), _selector(selector), _controller(controller)
  { _controller->add_param_listener(param_index, this); }
};

} // namespace inf::base::ui
#endif // INF_BASE_UI_LISTENERS_RELEVANCE_LISTENER_HPP