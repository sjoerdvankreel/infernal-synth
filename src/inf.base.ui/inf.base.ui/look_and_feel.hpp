#ifndef INF_BASE_UI_LOOK_AND_FEEL_HPP
#define INF_BASE_UI_LOOK_AND_FEEL_HPP

#include <juce_gui_basics/juce_gui_basics.h>

namespace inf::base::ui {

class inf_look_and_feel:
public juce::LookAndFeel_V4
{
public:
  enum colors {
    knob_highlight_low,
    knob_highlight_high,
    knob_outline_low,
    knob_outline_high,
    knob_outline_inactive,
    knob_cuts_inward_low,
    knob_cuts_outward_low,
    knob_cuts_inward_high,
    knob_cuts_outward_high,
    knob_thumb_inward_low,
    knob_thumb_outward_low,
    knob_thumb_inward_high,
    knob_thumb_outward_high,
    knob_center_stroke_low,
    knob_center_stroke_high,
    knob_spot_fill_base,
    knob_spot_fill_highlight,
    knob_gradient_fill_base,
    knob_gradient_fill_highlight,
  };

  void drawRotarySlider(
    juce::Graphics& g, int x0, int y0, int w0, int h0,
    float pos, float start, float end, juce::Slider& s) override;
};

} // namespace inf::base::ui
#endif // INF_BASE_UI_LOOK_AND_FEEL_HPP