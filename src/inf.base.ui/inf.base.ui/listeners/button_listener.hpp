#ifndef INF_BASE_UI_LISTENERS_BUTTON_LISTENER_HPP
#define INF_BASE_UI_LISTENERS_BUTTON_LISTENER_HPP

#include <inf.base/plugin/plugin_controller.hpp>
#include <juce_gui_basics/juce_gui_basics.h>
#include <functional>

namespace inf::base::ui {

class button_listener :
public juce::Button::Listener
{
  std::function<void()> _callback;
public:
  void buttonClicked(juce::Button*) override { _callback(); }
  button_listener(std::function<void()> callback): _callback(callback) {}
};

} // namespace inf::base::ui
#endif // INF_BASE_UI_LISTENERS_BUTTON_LISTENER_HPP