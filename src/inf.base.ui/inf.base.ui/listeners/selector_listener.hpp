#ifndef INF_BASE_UI_LISTENERS_SELECTOR_LISTENER_HPP
#define INF_BASE_UI_LISTENERS_SELECTOR_LISTENER_HPP

#include <inf.base/plugin/plugin_controller.hpp>
#include <inf.base.ui/controls/selector_bar.hpp>

namespace inf::base::ui {

class selector_listener :
public inf_selector_bar::listener,
public param_listener
{
  inf_selector_bar* const _bar;
  std::int32_t const _param_index;
  plugin_controller* const _controller;

public:
  void controller_param_changed(inf::base::param_value ui_value) override
  { _bar->setCurrentTabIndex(ui_value.discrete); }
  void tab_changed(inf_selector_bar* bar) override
  { _controller->editor_param_changed(_param_index, param_value(bar->getCurrentTabIndex())); }

  ~selector_listener() { _controller->remove_param_listener(_param_index, this); }
  selector_listener(plugin_controller* controller, inf_selector_bar* bar, std::int32_t param_index):
  _bar(bar), _param_index(param_index), _controller(controller) { _controller->add_param_listener(param_index, this); }
};

} // namespace inf::base::ui
#endif // INF_BASE_UI_LISTENERS_SELECTOR_LISTENER_HPP