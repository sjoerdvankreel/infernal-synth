#ifndef INF_BASE_UI_SHARED_LOOK_AND_FEEL_HPP
#define INF_BASE_UI_SHARED_LOOK_AND_FEEL_HPP

#include <inf.base.ui/shared/config.hpp>
#include <inf.base.ui/shared/support.hpp>
#include <juce_gui_basics/juce_gui_basics.h>
#include <inf.base/plugin/plugin_controller.hpp>

namespace inf::base::ui {

class inf_look_and_feel:
public juce::LookAndFeel_V4
{
  inf::base::plugin_controller const* const _controller;

  juce::Colour 
  with_enabled(juce::Component& component, std::int32_t color_id);

  void fill_spot_circle(juce::Graphics& g, 
    juce::Component& component, juce::Rectangle<float> rect,
    std::int32_t low_color_id, std::int32_t high_color_id);
  void fill_gradient_circle(juce::Graphics& g, 
    juce::Component& component, juce::Rectangle<float> rect,
    std::int32_t low_color_id, std::int32_t high_color_id);
  void fill_gradient_rounded_rectangle(juce::Graphics& g, 
    juce::Component& component, juce::Rectangle<float> rect, std::int32_t low_color_id, 
    std::int32_t high_color_id, float corner_size, float mid_point);

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
    knob_spot_fill_low,
    knob_spot_fill_high,
    knob_gradient_fill_low,
    knob_gradient_fill_high,

    slider_center_fill,
    slider_track_low,
    slider_track_high,
    slider_track_inactive,
    slider_highlight_low,
    slider_highlight_high,
    slider_spot_fill_low,
    slider_spot_fill_high,
    slider_gradient_fill_low,
    slider_gradient_fill_high,

    switch_outline_low,
    switch_outline_high,
    switch_spot_fill_low,
    switch_spot_fill_high,
    switch_gradient_fill_low_on,
    switch_gradient_fill_center_low,
    switch_gradient_fill_high_on,
    switch_gradient_fill_center_high,

    dropdown_outline_low,
    dropdown_outline_high,
    dropdown_background_low,
    dropdown_background_high,
    dropdown_tick_spot_fill_low,
    dropdown_tick_spot_fill_high,
    dropdown_tick_gradient_fill_low,
    dropdown_tick_gradient_fill_high,
    dropdown_highlight_background_low,
    dropdown_highlight_background_high,

    icon_stroke_color,
    icon_pw_stroke_color,

    group_label_color
  };

  inf_look_and_feel(inf::base::plugin_controller const* const controller):
  _controller(controller) {}

  // Disables ellipses and fitting.
  void drawLabel(
    juce::Graphics& g, juce::Label& label) override;

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
    juce::ComboBox& cb, juce::Label& l);
  void drawComboBox(
    juce::Graphics& g, int w0, int h0, bool isButtonDown,
    int button_x0, int button_y0, int button_w0, int button_h0, juce::ComboBox& cb) override;
  void drawPopupMenuBackground(
    juce::Graphics& g, int width, int height) override;
  void drawPopupMenuItem(
    juce::Graphics& g, juce::Rectangle<int> const& area,
    bool is_separator, bool is_active, bool is_highlighted, bool is_ticked, bool has_sub_menu,
    juce::String const& text, juce::String const& shortcut_key_text,
    juce::Drawable const* icon, juce::Colour const*) override;

  juce::Font getPopupMenuFont() override
  { return juce::Font(get_dropdown_font_height(_controller), juce::Font::bold); }
  juce::Font getComboBoxFont(juce::ComboBox& box) override
  { return juce::Font(get_dropdown_font_height(_controller), juce::Font::bold); }
  void getIdealPopupMenuItemSize(
    juce::String const& text, bool is_separator,
    int standard_menu_item_height, int& ideal_width, int& ideal_height) override
  { ideal_height = static_cast<int>(std::ceil(get_dropdown_item_height(_controller))); }
};

} // namespace inf::base::ui
#endif // INF_BASE_UI_SHARED_LOOK_AND_FEEL_HPP