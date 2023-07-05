#ifndef INF_BASE_UI_LISTENERS_TAB_BAR_PARAM_LISTENER_HPP
#define INF_BASE_UI_LISTENERS_TAB_BAR_PARAM_LISTENER_HPP

#include <juce_gui_basics/juce_gui_basics.h>
#include <inf.base/plugin/plugin_controller.hpp>

namespace inf::base::ui {

class tab_bar_param_listener :
public juce::ChangeListener,
public param_listener
{
  std::int32_t const _param_index;
  juce::TabbedButtonBar* const _bar;
  plugin_controller* const _controller;

public:
  void controller_param_changed(inf::base::param_value ui_value) override
  { _bar->setCurrentTabIndex(ui_value.discrete, juce::dontSendNotification); }
  void changeListenerCallback(juce::ChangeBroadcaster* source) override
  { _controller->editor_param_changed(_param_index, param_value(_bar->getCurrentTabIndex())); }

  ~tab_bar_param_listener() { _controller->remove_param_listener(_param_index, this); }
  tab_bar_param_listener(plugin_controller* controller, juce::TabbedButtonBar* bar, std::int32_t param_index):
  _param_index(param_index), _bar(bar), _controller(controller) { _controller->add_param_listener(param_index, this); }
};

} // namespace inf::base::ui
#endif // INF_BASE_UI_LISTENERS_TAB_BAR_PARAM_LISTENER_HPP