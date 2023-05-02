#ifndef INF_BASE_UI_SHARED_LOOK_AND_FEEL_HPP
#define INF_BASE_UI_SHARED_LOOK_AND_FEEL_HPP

#include <inf.base.ui/shared/config.hpp>
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

    slider_center_fill,
    slider_track_low,
    slider_track_high,
    slider_track_inactive,
    slider_highlight_low,
    slider_highlight_high,
    slider_spot_fill_base,
    slider_spot_fill_highlight,
    slider_gradient_fill_base,
    slider_gradient_fill_highlight,

    switch_outline_off,
    switch_outline_on_low,
    switch_outline_on_high,
    switch_spot_fill_base,
    switch_spot_fill_highlight,
    switch_gradient_fill_base_on,
    switch_gradient_fill_center_base,
    switch_gradient_fill_highlight_on,
    switch_gradient_fill_center_highlight,

    group_label_color
  };

  // Disables ellipses and fitting.
  void drawLabel(
    juce::Graphics& g, juce::Label& label) override;

  juce::Font getComboBoxFont(juce::ComboBox& box) override
  { return juce::Font(dropdown_font_height, juce::Font::bold); }

  // Custom slider.
  void drawLinearSlider(
    juce::Graphics& g, int x0, int y0, int w0, int h0,
    float pos0, float min0, float max0,
    juce::Slider::SliderStyle style, juce::Slider& s) override;

  // Custom knob.
  void drawRotarySlider(
    juce::Graphics& g, int x0, int y0, int w0, int h0,
    float pos, float start, float end, juce::Slider& s) override;

  // Custom on/off switch.
  void drawToggleButton(
    juce::Graphics& g, juce::ToggleButton& b,
    bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

  // Custom dropdown.
  void positionComboBoxText(
    juce::ComboBox& cb, juce::Label& l)
  { l.setBounds(cb.getLocalBounds()); }
  void drawComboBox(
    juce::Graphics& g, int w0, int h0, bool isButtonDown,
    int button_x0, int button_y0, int button_w0, int button_h0, juce::ComboBox& cb) override;
};

} // namespace inf::base::ui
#endif // INF_BASE_UI_SHARED_LOOK_AND_FEEL_HPP