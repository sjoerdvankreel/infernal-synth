#ifndef INF_BASE_UI_LISTENERS_COMBO_RELOAD_LISTENER_HPP
#define INF_BASE_UI_LISTENERS_COMBO_RELOAD_LISTENER_HPP

#include <inf.base/plugin/plugin_controller.hpp>
#include <juce_gui_basics/juce_gui_basics.h>
#include <functional>

namespace inf::base::ui {

class combo_reload_listener :
public inf::base::reload_listener
{
  juce::ComboBox* const _dropdown;
  std::function<void(juce::ComboBox*)> _callback;
  inf::base::plugin_controller* const _controller;
public:
  void plugin_reloaded() override { _callback(_dropdown); }
  ~combo_reload_listener() { _controller->remove_reload_param_listener(this); }
  combo_reload_listener(plugin_controller* controller, juce::ComboBox* dropdown, std::function<void(juce::ComboBox*)> const& callback):
  _controller(controller), _dropdown(dropdown), _callback(callback) { _controller->add_reload_listener(this); }
};

} // namespace inf::base::ui
#endif // INF_BASE_UI_LISTENERS_COMBO_RELOAD_LISTENER_HPP