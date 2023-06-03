#ifndef INF_BASE_UI_CONTROLS_BUTTON_HPP
#define INF_BASE_UI_CONTROLS_BUTTON_HPP

#include <juce_gui_basics/juce_gui_basics.h>

namespace inf::base::ui {

class inf_button:
public juce::TextButton
{
public:
  void paint(juce::Graphics& g) override;
};

} // namespace inf::base::ui
#endif // INF_BASE_UI_CONTROLS_BUTTON_HPP