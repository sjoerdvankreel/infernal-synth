#ifndef INF_BASE_UI_LISTENERS_TAB_PARAM_LISTENER_HPP
#define INF_BASE_UI_LISTENERS_TAB_PARAM_LISTENER_HPP

#include <inf.base/plugin/plugin_controller.hpp>
#include <inf.base.ui/controls/tabbed_button_bar.hpp>

namespace inf::base::ui {

class tab_param_listener :
public inf_tabbed_button_bar::listener,
public param_listener
{
  std::int32_t const _param_index;
  inf_tabbed_button_bar* const _bar;
  plugin_controller* const _controller;

public:
  void controller_param_changed(inf::base::param_value ui_value) override
  { _bar->setCurrentTabIndex(ui_value.discrete); }
  void tab_changed(inf_tabbed_button_bar* bar) override
  { _controller->editor_param_changed(_param_index, param_value(bar->getCurrentTabIndex())); }

  ~tab_param_listener() { _controller->remove_param_listener(_param_index, this); }
  tab_param_listener(plugin_controller* controller, inf_tabbed_button_bar* bar, std::int32_t param_index):
  _param_index(param_index), _bar(bar), _controller(controller) { _controller->add_param_listener(param_index, this); }
};

} // namespace inf::base::ui
#endif // INF_BASE_UI_LISTENERS_TAB_PARAM_LISTENER_HPP