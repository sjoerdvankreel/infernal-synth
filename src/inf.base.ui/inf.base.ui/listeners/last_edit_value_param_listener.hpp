#ifndef INF_BASE_UI_LISTENERS_LAST_EDIT_VALUE_PARAM_LISTENER_HPP
#define INF_BASE_UI_LISTENERS_LAST_EDIT_VALUE_PARAM_LISTENER_HPP

#include <inf.base/plugin/plugin_controller.hpp>
#include <juce_gui_basics/juce_gui_basics.h>

namespace inf::base::ui {

class last_edit_value_param_listener :
public any_param_listener
{
  juce::TextEditor* const _editor;
  plugin_controller* const _controller;

public:
  void any_controller_param_changed(std::int32_t index) override;
  ~last_edit_value_param_listener() { _controller->remove_any_param_listener(this); }
  last_edit_value_param_listener(plugin_controller* controller, juce::TextEditor* editor):
  _editor(editor), _controller(controller) { _controller->add_any_param_listener(this); }
};

} // namespace inf::base::ui
#endif // INF_BASE_UI_LISTENERS_LAST_EDIT_VALUE_PARAM_LISTENER_HPP