#ifndef INF_BASE_UI_LISTENERS_ACTION_DROPDOWN_LISTENER_HPP
#define INF_BASE_UI_LISTENERS_ACTION_DROPDOWN_LISTENER_HPP

#include <juce_gui_basics/juce_gui_basics.h>
#include <functional>
#include <cstdint>

namespace inf::base::ui {

class action_dropdown_listener :
public juce::ComboBox::Listener
{
  std::function<void(std::int32_t)> const _callback;
public:
  void comboBoxChanged(juce::ComboBox* dropdown) override;
  action_dropdown_listener(std::function<void(std::int32_t)> const& callback) : _callback(callback) {}
};

} // namespace inf::base::ui
#endif // INF_BASE_UI_LISTENERS_ACTION_DROPDOWN_LISTENER_HPP