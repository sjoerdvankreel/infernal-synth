#ifndef INF_BASE_UI_LOOK_AND_FEEL_HPP
#define INF_BASE_UI_LOOK_AND_FEEL_HPP

#include <juce_gui_basics/juce_gui_basics.h>

namespace inf::base::ui {

class inf_look_and_feel:
public juce::LookAndFeel_V4
{
public:
  enum colors {
    knob_thumb,
    knob_shadow,
    knob_outline,
    knob_highlight,
    knob_cuts_inward,
    knob_cuts_outward,
    knob_fill_base,
    knob_fill_highlight
  };

  void drawRotarySlider(
    juce::Graphics& g, int x, int y, int w, int h,
    float pos, float start, float end, juce::Slider& s) override;
};

} // namespace inf::base::ui
#endif // INF_BASE_UI_LOOK_AND_FEEL_HPP