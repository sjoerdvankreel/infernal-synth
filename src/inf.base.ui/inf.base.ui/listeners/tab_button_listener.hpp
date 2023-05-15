#ifndef INF_BASE_UI_LISTENERS_TAB_BUTTON_LISTENER_HPP
#define INF_BASE_UI_LISTENERS_TAB_BUTTON_LISTENER_HPP

#include <inf.base.ui/controls/tabbed_button_bar.hpp>
#include <functional>

namespace inf::base::ui {

class tab_button_listener :
public inf_tabbed_button_bar::listener
{
  std::function<void(inf_tabbed_button_bar*)> _callback;
public:
  void tab_changed(inf_tabbed_button_bar* bar) override { _callback(bar); }
  tab_button_listener(std::function<void(inf_tabbed_button_bar*)> callback): _callback(callback) {}
};

} // namespace inf::base::ui
#endif // INF_BASE_UI_LISTENERS_TAB_BUTTON_LISTENER_HPP