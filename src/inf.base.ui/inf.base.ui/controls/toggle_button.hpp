#ifndef INF_BASE_UI_CONTROLS_TOGGLE_BUTTON_HPP
#define INF_BASE_UI_CONTROLS_TOGGLE_BUTTON_HPP

#include <juce_gui_basics/juce_gui_basics.h>

namespace inf::base::ui {

class inf_toggle_button:
public juce::ToggleButton
{
  bool const _force_on;
public:
  bool force_on() const { return _force_on; }
  inf_toggle_button(bool force_on): 
  juce::ToggleButton(), _force_on(force_on) {}
};

} // namespace inf::base::ui
#endif // INF_BASE_UI_CONTROLS_TOGGLE_BUTTON_HPP