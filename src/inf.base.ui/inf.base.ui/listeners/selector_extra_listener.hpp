#ifndef INF_BASE_UI_LISTENERS_SELECTOR_EXTRA_LISTENER_HPP
#define INF_BASE_UI_LISTENERS_SELECTOR_EXTRA_LISTENER_HPP

#include <inf.base.ui/controls/selector_bar.hpp>
#include <functional>

namespace inf::base::ui {

class selector_extra_listener :
public inf_selector_bar::listener
{
  std::function<void(inf_selector_bar*)> _callback;
public:
  void tab_changed(inf_selector_bar* bar) override { _callback(bar); }
  selector_extra_listener(std::function<void(inf_selector_bar*)> callback): _callback(callback) {}
};

} // namespace inf::base::ui
#endif // INF_BASE_UI_LISTENERS_SELECTOR_EXTRA_LISTENER_HPP