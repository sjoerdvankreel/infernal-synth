#ifndef INF_BASE_UI_LISTENERS_ACTION_DROPDOWN_LISTENER_HPP
#define INF_BASE_UI_LISTENERS_ACTION_DROPDOWN_LISTENER_HPP

#include <juce_gui_basics/juce_gui_basics.h>
#include <functional>
#include <cstdint>

namespace inf::base::ui {

class action_dropdown_listener :
public juce::ComboBox::Listener
{
  std::function<void(juce::ComboBox*)> const _callback;
public:
  action_dropdown_listener(std::function<void(juce::ComboBox*)> const& callback) : _callback(callback) {}
  void comboBoxChanged(juce::ComboBox* dropdown) override { _callback(dropdown); }
};

} // namespace inf::base::ui
#endif // INF_BASE_UI_LISTENERS_ACTION_DROPDOWN_LISTENER_HPP