#ifndef INF_BASE_UI_SHARED_LOOK_AND_FEEL_HPP
#define INF_BASE_UI_SHARED_LOOK_AND_FEEL_HPP

#include <inf.base.ui/shared/config.hpp>
#include <inf.base.ui/shared/support.hpp>
#include <juce_gui_basics/juce_gui_basics.h>
#include <inf.base/plugin/plugin_controller.hpp>

#include <vector>
#include <memory>

namespace inf::base::ui {

class inf_look_and_feel:
public juce::LookAndFeel_V4
{
  inf::base::plugin_controller* const _controller;
  std::map<std::string, std::string> _graph_bg_images = {};

  void
  try_load_theme_color(std::int32_t color, juce::var const& colors, char const* name);

  void 
  draw_popup_base(
    juce::Graphics& g, int w0, int h0, bool isButtonDown,
    int button_x0, int button_y0, int button_w0, int button_h0, juce::ComboBox& cb, float corner_size);
  
  juce::Colour
  with_enabled(juce::Component& component, std::int32_t color_id, bool check_enabled = true, bool extra_dark = false);

  juce::ColourGradient gradient_fill(
    juce::Component& component, juce::Rectangle<float> rect, 
    std::int32_t low_color_id, std::int32_t high_color_id, float mid_point, bool check_enabled = true, bool extra_dark = false);

public:

  // Be sure to keep in sync with theme files and theme loading in constructor.
  enum colors {

    param_label,
    part_group_label,
    root_background,
    
    icon_stroke_color,
    icon_pw_stroke_color,

    text_edit_text,
    text_edit_outline_low,
    text_edit_outline_high,
    text_edit_background_low,
    text_edit_background_high,

    tooltip_text,
    tooltip_outline_low,
    tooltip_outline_high,
    tooltip_background_low,
    tooltip_background_high,

    dialog_text,
    dialog_header_text,
    dialog_outline_low,
    dialog_outline_high,
    dialog_background_low,
    dialog_background_high,
    
    file_box_title,
    file_box_file_text,
    file_box_background,
    file_box_file_background,
    file_box_label_text,
    file_box_label_background,
    file_box_button_text,
    file_box_button_background,
    file_box_selector_text,
    file_box_selector_highlight,
    file_box_selector_highlight_text,

    selector_label_text,
    selector_label_outline_low,
    selector_label_outline_high,
    selector_label_background_low,
    selector_label_background_high,

    part_graph_grid,
    part_graph_area,
    part_graph_line,
    part_graph_fill_low,
    part_graph_fill_high,
    part_graph_outline_low,
    part_graph_outline_high,

    part_group_container_fill_low,
    part_group_container_fill_high,
    part_group_container_outline_low,
    part_group_container_outline_high,

    button_text,
    button_outline_low,
    button_outline_high,
    button_background_low,
    button_background_high,
    button_over_background_low,
    button_over_background_high,
    button_down_background_low,
    button_down_background_high,

    tab_button_text,
    tab_button_outline_low,
    tab_button_outline_high,
    tab_button_background_low,
    tab_button_background_high,
    tab_button_highlight_background_low,
    tab_button_highlight_background_high,

    switch_outline_low,
    switch_outline_high,
    switch_spot_fill_low,
    switch_spot_fill_high,
    switch_gradient_fill_low_on,
    switch_gradient_fill_center_low,
    switch_gradient_fill_high_on,
    switch_gradient_fill_center_high,

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

    dropdown_text,
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
  };

  inf_look_and_feel(
    inf::base::plugin_controller* controller, 
    std::vector<std::string> theme_color_sections);

  std::string
  get_graph_bg_image_path(std::string const& image_id) { return _graph_bg_images[image_id]; }

  // Shared paint routines.
  void fill_spot_circle(juce::Graphics& g,
    juce::Component& component, juce::Rectangle<float> rect,
    std::int32_t low_color_id, std::int32_t high_color_id, bool check_enabled = true, bool extra_dark = false);
  void fill_gradient_circle(juce::Graphics& g,
    juce::Component& component, juce::Rectangle<float> rect,
    std::int32_t low_color_id, std::int32_t high_color_id, bool check_enabled = true, bool extra_dark = false);
  void fill_gradient_rounded_rectangle(juce::Graphics& g,
    juce::Component& component, juce::Rectangle<float> rect, std::int32_t low_color_id,
    std::int32_t high_color_id, float corner_size, float mid_point, bool check_enabled = true, bool extra_dark = false);
  void stroke_gradient_rounded_rectangle(juce::Graphics& g,
    juce::Component& component, juce::Rectangle<float> rect, std::int32_t low_color_id,
    std::int32_t high_color_id, float corner_size, float mid_point, float line_size, bool check_enabled = true, bool extra_dark = false);

  // Disables ellipses and fitting.
  void drawLabel(
    juce::Graphics& g, juce::Label& label) override;

  // Tabs.
  int getTabButtonBestWidth(
    juce::TabBarButton& button, int tab_depth) override;
  void drawTabButton(
    juce::TabBarButton& tbb, juce::Graphics& g, 
    bool is_mouse_over, bool is_mouse_down) override;
  void drawTabAreaBehindFrontButton(
    juce::TabbedButtonBar& bar, juce::Graphics& g, 
    const int w, const int h) override {}

  // Tooltip.
  void drawTooltip(
    juce::Graphics& g, juce::String const& text, 
    int w0, int h0) override;

  // Dialog.
  void drawAlertBox(
    juce::Graphics& g, juce::AlertWindow& w, 
    juce::Rectangle<int> const& text_area, juce::TextLayout& l) override;

  // Textbox.
  void fillTextEditorBackground(
    juce::Graphics&, int width, int height, juce::TextEditor&) override;
  void drawTextEditorOutline(
    juce::Graphics&, int width, int height, juce::TextEditor&) override;

  // Sliders/knobs.
  void drawLinearSlider(
    juce::Graphics& g, int x0, int y0, int w0, int h0,
    float pos0, float min0, float max0,
    juce::Slider::SliderStyle style, juce::Slider& s) override;
  void drawRotarySlider(
    juce::Graphics& g, int x0, int y0, int w0, int h0,
    float pos, float start, float end, juce::Slider& s) override;
  void drawToggleButton(
    juce::Graphics& g, juce::ToggleButton& b,
    bool draw_as_highlighted, bool draw_as_down) override;

  // File dialog.
  void layoutFileBrowserComponent(
    juce::FileBrowserComponent& browser_comp,
    juce::DirectoryContentsDisplayComponent* file_list_component,
    juce::FilePreviewComponent* preview_comp,
    juce::ComboBox* current_path_box,
    juce::TextEditor* filename_box,
    juce::Button* go_up_button);
  void drawFileBrowserRow(
    juce::Graphics& g, int width, int height,
    juce::File const& file, juce::String const& file_name, juce::Image* icon,
    juce::String const& file_size_description,
    juce::String const& file_time_description,
    bool is_directory, bool is_item_selected,
    int item_index, juce::DirectoryContentsDisplayComponent& dcc);

  // Dropdown.
  void positionComboBoxText(
    juce::ComboBox& cb, juce::Label& l) override;
  juce::PopupMenu::Options getOptionsForComboBoxPopupMenu(
    juce::ComboBox& b, juce::Label& l) override;
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
  void getIdealPopupMenuItemSize(
    juce::String const& text, bool is_separator,
    int standard_menu_item_height, int& ideal_width, int& ideal_height) override;

  juce::Font getPopupMenuFont() override
  { return juce::Font(get_dropdown_font_height(_controller), juce::Font::bold); }
  juce::Font getComboBoxFont(juce::ComboBox& box) override
  { return juce::Font(get_dropdown_font_height(_controller), juce::Font::bold); }
};

typedef std::unique_ptr<inf_look_and_feel> (*
lnf_factory)(inf::base::plugin_controller*);

} // namespace inf::base::ui
#endif // INF_BASE_UI_SHARED_LOOK_AND_FEEL_HPP