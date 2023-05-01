#ifndef INF_BASE_UI_LISTENERS_DROPDOWN_PARAM_LISTENER_HPP
#define INF_BASE_UI_LISTENERS_DROPDOWN_PARAM_LISTENER_HPP

#include <inf.base.ui/controls/dropdown.hpp>
#include <juce_gui_basics/juce_gui_basics.h>
#include <inf.base/plugin/plugin_controller.hpp>

namespace inf::base::ui 
{

class dropdown_param_listener :
public juce::ComboBox::Listener,
public param_listener
{
  juce::ComboBox* const _dropdown;
  std::int32_t const _param_index;
  plugin_controller* const _controller;

public:
  void comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) override {}
  void controller_param_changed(inf::base::param_value ui_value) override {}

  ~dropdown_param_listener() { _controller->remove_param_listener(_param_index, this); }
  dropdown_param_listener(plugin_controller* controller, juce::ComboBox* dropdown, std::int32_t param_index):
    _dropdown(dropdown), _param_index(param_index), _controller(controller) { _controller->add_param_listener(param_index, this); }
};

} // namespace inf::base::ui
#endif // INF_BASE_UI_LISTENERS_DROPDOWN_PARAM_LISTENER_HPP