#ifndef INF_BASE_UI_LISTENERS_DROPDOWN_PARAM_LISTENER_HPP
#define INF_BASE_UI_LISTENERS_ANY_RELOAD_LISTENER_HPP

#include <inf.base/plugin/plugin_controller.hpp>
#include <functional>

namespace inf::base::ui {

class any_reload_listener :
public inf::base::reload_listener
{
  std::function<void()> _callback;
  inf::base::plugin_controller* const _controller;
public:
  void plugin_reloaded() override { _callback(); }
  ~any_reload_listener() { _controller->remove_reload_param_listener(this); }
  any_reload_listener(plugin_controller* controller, std::function<void()> const& callback):
  _controller(controller), _callback(_callback) { _controller->add_reload_listener(this); }
};

} // namespace inf::base::ui
#endif // INF_BASE_UI_LISTENERS_ANY_RELOAD_LISTENER_HPP