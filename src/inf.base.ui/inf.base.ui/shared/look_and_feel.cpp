#include <inf.base/shared/support.hpp>
#include <inf.base.ui/shared/look_and_feel.hpp>
#include <inf.base.ui/controls/slider.hpp>
#include <inf.base.ui/controls/toggle_button.hpp>

using namespace juce;

namespace inf::base::ui {

juce::Colour
inf_look_and_feel::with_enabled(Component& component, std::int32_t color_id, bool check_enabled)
{
  auto result = component.findColour(color_id);
  if(!check_enabled || component.isEnabled()) return result;
  std::uint8_t gray = static_cast<std::uint8_t>(
    std::round(result.getRed() * 0.299 + 
    result.getGreen() * 0.587 + 
    result.getBlue() * 0.114));
  return Colour(gray, gray, gray, result.getAlpha());
}

juce::ColourGradient 
inf_look_and_feel::gradient_fill(
  juce::Component& component, juce::Rectangle<float> rect, 
  std::int32_t low_color_id, std::int32_t high_color_id, float mid_point, bool check_enabled)
{
  auto low = with_enabled(component, low_color_id, check_enabled);
  auto high = with_enabled(component, high_color_id, check_enabled);
  auto gradient = ColourGradient(high, rect.getTopLeft(), low, rect.getBottomRight(), false);
  gradient.addColour(mid_point, high.interpolatedWith(low, 0.5f));
  return gradient;
}

void 
inf_look_and_feel::fill_gradient_circle(
  Graphics& g, Component& component, juce::Rectangle<float> rect,
  std::int32_t low_color_id, std::int32_t high_color_id, bool check_enabled)
{
  g.setGradientFill(gradient_fill(component, rect, low_color_id, high_color_id, 0.25f, check_enabled));
  g.fillEllipse(rect);
}

void 
inf_look_and_feel::fill_gradient_rounded_rectangle(
  juce::Graphics& g, Component& component, juce::Rectangle<float> rect, 
  std::int32_t low_color_id, std::int32_t high_color_id, float corner_size, float mid_point, bool check_enabled)
{
  g.setGradientFill(gradient_fill(component, rect, low_color_id, high_color_id, mid_point, check_enabled));
  g.fillRoundedRectangle(rect, corner_size);
}

void
inf_look_and_feel::stroke_gradient_rounded_rectangle(
  juce::Graphics& g, Component& component, juce::Rectangle<float> rect,
  std::int32_t low_color_id, std::int32_t high_color_id, float corner_size, float mid_point, float line_size, bool check_enabled)
{
  g.setGradientFill(gradient_fill(component, rect, low_color_id, high_color_id, mid_point, check_enabled));
  g.drawRoundedRectangle(rect, corner_size, line_size);
}

void
inf_look_and_feel::fill_spot_circle(
  Graphics& g, Component& component, Rectangle<float> rect,
  std::int32_t low_color_id, std::int32_t high_color_id, bool check_enabled)
{
  float const size_factor = 0.67f;
  float const size = size_factor * rect.getWidth();
  float const offset = (rect.getWidth() - size) * 0.5f;
  float const x = rect.getX() + offset * 0.5f;
  float const y = rect.getY() + offset * 0.5f;
  float const center_x = x + size * 0.5f;
  float const center_y = y + size * 0.5f;
  auto low = with_enabled(component, low_color_id, check_enabled);
  auto high = with_enabled(component, high_color_id, check_enabled);
  auto gradient = ColourGradient(
    high, center_x, center_y,
    low, rect.getX() + size, rect.getY() + size, true);
  gradient.addColour(0.25, high.interpolatedWith(low, 0.5f));
  g.setGradientFill(gradient);
  g.fillEllipse(x, y, size, size);
}

void 
inf_look_and_feel::drawLabel(Graphics& g, Label& label)
{
  auto const& font = getLabelFont(label);
  g.fillAll(with_enabled(label, Label::backgroundColourId));
  g.setFont(font);
  g.setColour(with_enabled(label, Label::textColourId));
  auto text_area = getLabelBorderSize(label).subtractedFrom(label.getLocalBounds());
  g.drawText(label.getText(), text_area, label.getJustificationType(), false);
}

void 
inf_look_and_feel::drawTooltip(
  juce::Graphics& g, juce::String const& text, int w0, int h0)
{
  float const corner_size = 0.0f;
  float const w = static_cast<float>(w0);
  float const h = static_cast<float>(h0);

  juce::Component dummy;
  dummy.setLookAndFeel(this);
  fill_gradient_rounded_rectangle(g, dummy, Rectangle<float>(0.0f, 0.0f, w, h), 
    colors::tooltip_background_low, colors::tooltip_background_high, corner_size, 0.25f);
  stroke_gradient_rounded_rectangle(g, dummy, Rectangle<float>(0.0f, 0.0f, w, h),
    colors::tooltip_outline_low, colors::tooltip_outline_high, corner_size, 0.25f, 2.0f);
  g.setColour(findColour(TooltipWindow::ColourIds::textColourId));
  g.setFont(getPopupMenuFont());
  g.drawText(text, 0, 0, w0, h0, Justification::centred, false);
} 

int 
inf_look_and_feel::getTabButtonBestWidth(
  TabBarButton& button, int tab_depth)
{
  int width = button.getTabbedButtonBar().getWidth();
  int count = button.getTabbedButtonBar().getNumTabs();
  return width / count - 1;
}

void 
inf_look_and_feel::drawTabButton(
  juce::TabBarButton& tbb, juce::Graphics& g,
  bool is_mouse_over, bool is_mouse_down)
{
  // config
  int const padding = 2;
  float const corner_size_fixed = 5.0f;
  float const outline_size_fixed = 1.0f;

  auto area = Rectangle<int>(
    tbb.getActiveArea().getX() + padding / 2,
    tbb.getActiveArea().getY() + padding / 2,
    tbb.getActiveArea().getWidth() - padding,
    tbb.getActiveArea().getHeight() - padding);

  // fill
  std::int32_t background_low = colors::tab_button_background_low;
  std::int32_t background_high = colors::tab_button_background_high;
  if (tbb.getIndex() == tbb.getTabbedButtonBar().getCurrentTabIndex())
  {
    background_low = colors::tab_button_highlight_background_low;
    background_high = colors::tab_button_highlight_background_high;
  }
  fill_gradient_rounded_rectangle(
    g, tbb, area.toFloat(), background_low,
    background_high, corner_size_fixed, 0.25f);

  // outline
  stroke_gradient_rounded_rectangle(
    g, tbb, area.toFloat(), colors::tab_button_outline_low,
    colors::tab_button_outline_high, corner_size_fixed, 0.25f, outline_size_fixed);

  // text
  g.setFont(juce::Font(get_selector_font_height(_controller), juce::Font::bold));
  g.setColour(findColour(TabbedButtonBar::ColourIds::tabTextColourId));
  g.drawText(tbb.getName(), area, Justification::centred, false);
}

void 
inf_look_and_feel::drawToggleButton(
  juce::Graphics& g, juce::ToggleButton& b,
  bool draw_as_highlighted, bool draw_as_down)
{
  // config
  float const margin_factor = 0.05f;
  float const center_size_factor = 0.67f;
  float const outline_thickness_factor = 0.075f;

  // precompute stuff
  int const x0 = b.getLocalBounds().getX();
  int const y0 = b.getLocalBounds().getY();
  int const w0 = b.getLocalBounds().getWidth();
  int const h0 = b.getLocalBounds().getHeight();
  float x = static_cast<float>(x0);
  float y = static_cast<float>(y0);
  float w = static_cast<float>(w0);
  float h = static_cast<float>(h0);
  bool const on = b.getToggleState();
  auto& inf_toggle = dynamic_cast<inf_toggle_button&>(b);

  // adjust for nonrectangular
  float minwh = std::min(w, h);
  if (minwh < w) 
  {
    x = (w - minwh) / 2.0f;
    w = minwh;
  } else
  {
    y = (h - minwh) / 2.0f;
    h = minwh;
  }

  // precompute more stuff
  x += margin_factor * w;
  y += margin_factor * h;
  w -= margin_factor * w * 2.0f;
  h -= margin_factor * h * 2.0f;  
  float const center_size = w * center_size_factor;

  // outline
  Path outline;
  float const outline_thickness = w * outline_thickness_factor;
  auto outline_low = with_enabled(b, colors::switch_outline_low);
  auto outline_high = with_enabled(b, colors::switch_outline_high);
  auto outline_gradient = ColourGradient(outline_high, x, y, outline_low, x + w, y + h, false);
  outline_gradient.addColour(0.33, outline_high.interpolatedWith(outline_low, 0.5f));
  g.setGradientFill(outline_gradient);
  outline.addEllipse(x, y, w, h);
  g.strokePath(outline, PathStrokeType(outline_thickness));

  // fill
  float const fill_offset = (w - center_size) * 0.5f;
  Rectangle<float> fill_rect(x + fill_offset, y + fill_offset, center_size, center_size);
  fill_gradient_circle(g, b, fill_rect, colors::switch_gradient_fill_center_low, colors::switch_gradient_fill_center_high, !inf_toggle.force_on());
  if(on || inf_toggle.force_on()) fill_gradient_circle(g, b, fill_rect, colors::switch_gradient_fill_low_on, colors::switch_gradient_fill_high_on, !inf_toggle.force_on());
  fill_spot_circle(g, b, fill_rect, colors::switch_spot_fill_low, colors::switch_spot_fill_high, !inf_toggle.force_on());
}

// Custom dropdown.
void 
inf_look_and_feel::positionComboBoxText(
  juce::ComboBox& cb, juce::Label& l)
{
  l.setBounds(cb.getLocalBounds());
  l.setFont(getComboBoxFont(cb));
}

PopupMenu::Options 
inf_look_and_feel::getOptionsForComboBoxPopupMenu(ComboBox& b, Label& l)
{
  float items = static_cast<float>(b.getNumItems());
  float fcolumns = items / dropdown_max_column_items;
  int columns = static_cast<std::int32_t>(std::ceil(fcolumns));
  return LookAndFeel_V4::getOptionsForComboBoxPopupMenu(b, l).withMinimumNumColumns(columns);
}

void 
inf_look_and_feel::drawPopupMenuBackground(
  juce::Graphics& g, int width, int height) 
{
  juce::ComboBox dummy;
  dummy.setLookAndFeel(this);
  drawComboBox(g, width, height, false, 0, 0, 0, 0, dummy);
}

void 
inf_look_and_feel::drawComboBox(
  Graphics& g, int w0, int h0, bool isButtonDown,
  int button_x0, int button_y0, int button_w0, int button_h0, ComboBox& cb)
{
  // config
  float const corner_size_fixed = 5.0f;
  float const outline_size_fixed = 1.0f;

  // precompute stuff
  float const x = static_cast<float>(0);
  float const y = static_cast<float>(0);
  float const w = static_cast<float>(w0);
  float const h = static_cast<float>(h0);

  fill_gradient_rounded_rectangle(
    g, cb, Rectangle<float>(x, y, w, h), colors::dropdown_background_low, 
    colors::dropdown_background_high, corner_size_fixed, 0.25f);
  
  // outline
  auto outline_low = with_enabled(cb, colors::dropdown_outline_low);
  auto outline_high = with_enabled(cb, colors::dropdown_outline_high);
  auto outline_gradient = ColourGradient(outline_high, x, y, outline_low, x + w, y + h, false);
  outline_gradient.addColour(0.25, outline_high.interpolatedWith(outline_low, 0.5f));
  g.setGradientFill(outline_gradient);
  g.drawRoundedRectangle(
    x + outline_size_fixed / 2.0f, y + outline_size_fixed / 2.0f,
    w - outline_size_fixed, h - outline_size_fixed, 
    corner_size_fixed, outline_size_fixed);
}

void 
inf_look_and_feel::drawPopupMenuItem(
  juce::Graphics& g, juce::Rectangle<int> const& area0,
  bool is_separator, bool is_active, bool is_highlighted, bool is_ticked, bool has_sub_menu,
  juce::String const& text, juce::String const& shortcut_key_text,
  juce::Drawable const* icon, juce::Colour const*)
{
  Component dummy;
  dummy.setLookAndFeel(this);
  
  // config
  float const padding_fixed = 2.0f;
  float const text_hpad_fixed = 8.0f;
  float const tick_size_factor = 0.9f;
  float const corner_size_fixed = 5.0f;

  auto farea = area0.toFloat();
  juce::Rectangle<float> hl_rect(
    farea.getX() + padding_fixed / 2.0f,
    farea.getY() + padding_fixed / 2.0f,
    farea.getWidth() - padding_fixed,
    farea.getHeight() - padding_fixed);

  // hover bg
  if(is_highlighted)
    fill_gradient_rounded_rectangle(
      g, dummy, hl_rect, colors::dropdown_highlight_background_low,
      colors::dropdown_highlight_background_high, corner_size_fixed, 0.25f);

  // tick
  juce::Rectangle<float> tick_rect(
    hl_rect.getX() + text_hpad_fixed / 2.0f, 
    hl_rect.getY() + text_hpad_fixed / 2.0f,
    hl_rect.getHeight() - text_hpad_fixed, 
    hl_rect.getHeight() - text_hpad_fixed);
  if(is_ticked)
  {
    juce::Rectangle<float> tick_rect2(
      tick_rect.getX() + (1.0f - tick_size_factor) / 2.0f * tick_rect.getWidth(),
      tick_rect.getY() + (1.0f - tick_size_factor) / 2.0f * tick_rect.getHeight(),
      tick_rect.getWidth() * tick_size_factor,
      tick_rect.getHeight() * tick_size_factor);
    fill_gradient_circle(g, dummy, tick_rect2, colors::dropdown_tick_gradient_fill_low, colors::dropdown_tick_gradient_fill_high);
    fill_spot_circle(g, dummy, tick_rect2, colors::dropdown_tick_spot_fill_low, colors::dropdown_tick_spot_fill_high);
  }

  // text
  juce::Rectangle<float> text_rect(
    tick_rect.getX() + tick_rect.getWidth(), hl_rect.getY(),
    hl_rect.getWidth() - tick_rect.getWidth(), hl_rect.getHeight());
  g.setColour(with_enabled(dummy, ComboBox::ColourIds::textColourId));
  g.setFont(getPopupMenuFont());
  g.drawText(text, text_rect, Justification::centredLeft, false);
  
  // submenu indicator
  if (has_sub_menu)
  {
    Path path;
    auto r = area0.reduced(3);
    auto arrow_h = 0.6f * getPopupMenuFont().getAscent();
    auto x = static_cast<float>(r.removeFromRight(static_cast<int>(arrow_h)).getX());
    auto half_h = static_cast<float> (r.getCentreY());
    path.startNewSubPath(x, half_h - arrow_h * 0.5f);
    path.lineTo(x + arrow_h * 0.6f, half_h);
    path.lineTo(x, half_h + arrow_h * 0.5f);
    g.strokePath(path, PathStrokeType(2.0f));
  }
}

void 
inf_look_and_feel::drawLinearSlider(
  Graphics& g, int x0, int y0, int w0, int h0,
  float pos0, float min0, float max0,
  Slider::SliderStyle style, Slider& s)
{  
  // config
  float const margin_factor = 0.0f;
  float const thumb_xy_ratio = 3.0;
  float const thumb_size_factor = 3.0f;
  float const track_size_factor = 0.25f;
  float const track_inner_size_factor = 0.8f;
  bool const vertical = style == Slider::SliderStyle::LinearVertical;
  auto const& desc = dynamic_cast<inf_slider const&>(s).descriptor();
  bool const bipolar = desc->data.type == param_type::real ? desc->data.real.display.min < 0.0f : desc->data.discrete.min < 0;

  // precompute stuff
  float x = static_cast<float>(x0);
  float y = static_cast<float>(y0);
  float w = static_cast<float>(w0);
  float h = static_cast<float>(h0);
  x += margin_factor * w;
  y += margin_factor * h;
  w -= 2.0f * margin_factor * w;
  h -= 2.0f * margin_factor * h;
  float const small = vertical? w: h;
  float const start_x = vertical? x + w / 2.0f: x;
  float const start_y = vertical? y: y + h / 2.0f;
  float const end_x = vertical? x + w / 2.0f : x + w;
  float const end_y = vertical? y + h: y + h / 2.0f;
  float const pos = vertical? (pos0 - y) / h: (pos0 - x) / w;
  float const pos_x = vertical ? start_x : x + pos * w;
  float const pos_y = vertical ? y + pos * h : start_y;
  float const active_end_y = vertical ? y : end_y;
  float const active_start_y = vertical? y + h: start_y;
  float const active_pos_y = vertical? y + pos * h: pos_y;
  float const mid_x = vertical ? start_x: x + w / 2.0f;
  float const mid_y = vertical ? start_y + h / 2.0f : start_y;

  // track inactive gradient
  float track_size = small * track_size_factor;
  juce::Rectangle<float> track_rect(
    start_x - track_size / 2.0f, 
    start_y - track_size / 2.0f, 
    vertical ? track_size : w + track_size, 
    vertical ? h + track_size : track_size);
  fill_gradient_rounded_rectangle(
    g, s, track_rect, colors::slider_highlight_high, 
    colors::slider_highlight_low, track_size / 2.0f, 0.75f);

  // track inactive fill
  float track_inner_size = track_size * track_inner_size_factor;
  float track_inner_offset = track_size - track_inner_size;
  juce::Rectangle<float> track_inner_rect(
    track_rect.getX() + track_inner_offset / 2.0f, 
    track_rect.getY() + track_inner_offset / 2.0f, 
    track_rect.getWidth() - track_inner_offset, 
    track_rect.getHeight() - track_inner_offset);
  g.setColour(with_enabled(s, colors::slider_track_inactive));
  g.fillRoundedRectangle(track_inner_rect, track_inner_size / 2.0f);

  // track active
  Path track_active;
  auto track_low = with_enabled(s, colors::slider_track_low);
  auto track_high = with_enabled(s, colors::slider_track_high);
  if(bipolar)
  {
    track_active.startNewSubPath(Point<float>(mid_x, mid_y));
    track_active.lineTo(Point<float>(pos_x, active_pos_y));
    float const gradient_x = pos >= 0.5f? end_x: start_x;
    float const gradient_y = (vertical && pos >= 0.5f) || (!vertical && pos < 0.5f)? active_start_y: active_end_y;
    g.setGradientFill(ColourGradient(track_low, mid_x, mid_y, track_high, gradient_x, gradient_y, false));
  } else
  {
    track_active.startNewSubPath(Point<float>(start_x, active_start_y));
    track_active.lineTo(Point<float>(pos_x, active_pos_y));
    g.setGradientFill(ColourGradient(track_low, start_x, active_start_y, track_high, end_x, active_end_y, false));
  }
  g.strokePath(track_active, { track_inner_size, PathStrokeType::curved, bipolar? PathStrokeType::butt: PathStrokeType::rounded });

  // thumb fill + gradient overlay
  float const thumb_large = track_size * thumb_size_factor;
  float const thumb_small = thumb_large / thumb_xy_ratio;
  juce::Rectangle<float> thumb_rect(
    pos_x - 0.5f * (vertical? thumb_large: thumb_small), 
    pos_y - 0.5f * (vertical? thumb_small: thumb_large), 
    vertical? thumb_large: thumb_small, 
    vertical? thumb_small: thumb_large);
  g.setColour(with_enabled(s, colors::slider_center_fill));
  g.fillRoundedRectangle(thumb_rect, thumb_small / 2.0f);
  fill_gradient_rounded_rectangle(
    g, s, thumb_rect, colors::slider_gradient_fill_low, 
    colors::slider_gradient_fill_high, thumb_small / 2.0f, 0.25f);

  // thumb spot
  Rectangle<float> spot_rect(thumb_rect.getX(), thumb_rect.getY(), thumb_small, thumb_small);
  fill_spot_circle(g, s, spot_rect, colors::slider_spot_fill_low, colors::slider_spot_fill_high);
}

void 
inf_look_and_feel::drawRotarySlider(
  juce::Graphics& g, int x0, int y0, int w0, int h0, 
  float pos, float start_angle, float end_angle, juce::Slider& s)
{
  // config
  float const cut_xy_ratio = 1.0f;
  float const margin_factor = 0.075f;
  float const margin_fixed_px = 2.0f;
  float const thumb_width_factor = 0.15f;
  float const center_size_factor = 0.67f;
  float const cut_inner_size_factor = 0.85f;
  float const highlight_size_factor = 0.75f;
  float const outer_xy_offset_factor = 0.95f;
  float const outline_step_gap_factor = 0.015f;
  float const center_thickness_factor = 0.0125f;
  float const outline_thickness_factor = 0.075f;
  float const cut_line_thickness_factor = 0.0125f;

  // config
  std::int32_t step_count = 0;
  std::int32_t const cut_count = 10;
  std::int32_t const fake_conic_gradient_count = 1024;
  auto const& is = dynamic_cast<inf_slider const&>(s);
  auto const descriptor = is.descriptor();
  bool const outline_gradient = is.type() == edit_type::knob;
  bool const bipolar = descriptor->data.type == param_type::real ? descriptor->data.real.display.min < 0.0f : descriptor->data.discrete.min < 0;
  if(is.type() == edit_type::selector)
  {
    assert(s.getInterval() > 0.0);
    step_count = static_cast<std::int32_t>(std::round(s.getMaximum() - s.getMinimum()));
  }

  // adjust for nonrectangular
  float w = static_cast<float>(w0);
  float h = static_cast<float>(h0);
  float top = static_cast<float>(y0);
  float left = static_cast<float>(x0);
  if (w < h) top += (h - w) * 0.5f;
  if (h < w) left += (w - h) * 0.5f;

  // then for outer offset (we're more wide then tall due to gap at bottom)
  top += (1.0f - outer_xy_offset_factor) * 0.5f * h;
  left -= (1.0f - outer_xy_offset_factor) * 0.5f * w;
  w += (1.0f - outer_xy_offset_factor) * std::min(w, h);
  h += (1.0f - outer_xy_offset_factor) * std::min(w, h);

  // precompute stuff
  float const margin = std::min(w, h) * margin_factor + margin_fixed_px;
  float const outer_size = std::min(w, h) - 2.0f * margin;
  float const thumb_width = outer_size * thumb_width_factor;
  float const highlight_size = outer_size * highlight_size_factor;
  float const center_size = highlight_size * center_size_factor;
  float const angle = start_angle + pos * (end_angle - start_angle) - pi32 * 0.5f;

  // precompute bounds
  float const outer_y = top + margin;
  float const outer_x = left + margin;
  float const center_y = top + margin + outer_size / 2.0f;
  float const center_x = left + margin + outer_size / 2.0f;

  // inactive outline
  float const outer_radius = outer_size / 2.0f;
  float const outline_thickness = outer_size * outline_thickness_factor;
  g.setColour(with_enabled(s, colors::knob_outline_inactive));
  if(step_count == 0)
  {
    Path inactive_outline;
    inactive_outline.addCentredArc(center_x, center_y, outer_radius, outer_radius, 0.0f, start_angle, end_angle, true);
    g.strokePath(inactive_outline, PathStrokeType(outline_thickness));
  }
  else for (std::int32_t sc = 0; sc < step_count; sc++)
  {
    Path inactive_outline;
    float fsc = static_cast<float>(sc);
    float this_step = fsc / step_count;
    float next_step = (fsc + 1.0f) / step_count;
    float range = end_angle - start_angle;
    float half_gap = outline_step_gap_factor * 0.5f;
    float const this_start_angle = sc == 0? start_angle: start_angle + (this_step + half_gap) * range;
    float const this_end_angle = sc == step_count - 1 ? end_angle : start_angle + (next_step - half_gap) * range;
    inactive_outline.addCentredArc(center_x, center_y, outer_radius, outer_radius, 0.0f, this_start_angle, this_end_angle, true);
    g.strokePath(inactive_outline, PathStrokeType(outline_thickness));
  }

  // active outline
  for (std::int32_t i = 0; i < fake_conic_gradient_count; i++)
  {
    Path active_outline;
    float fi = static_cast<float>(i);
    float fstep_count = static_cast<float>(step_count);
    float this_pos = fi / fake_conic_gradient_count;
    if(!bipolar && this_pos >= pos) break;
    if (bipolar && this_pos <= 0.5f && pos >= this_pos) continue;
    if (bipolar && this_pos >= 0.5f && pos <= this_pos) continue;
    float active_start_angle = start_angle + fi / fake_conic_gradient_count * (end_angle - start_angle);
    float active_end_angle = start_angle + (fi + 1.0f) / fake_conic_gradient_count * (end_angle - start_angle);
    active_outline.addCentredArc(center_x, center_y, outer_radius, outer_radius, 0.0f, active_start_angle, active_end_angle, true);
    auto active_outline_low = with_enabled(s, colors::knob_outline_low);
    auto active_outline_high = with_enabled(s, colors::knob_outline_high);
    float mix_factor = (i + 1.0f) / fake_conic_gradient_count;
    if(bipolar) mix_factor = std::abs(mix_factor * 2.0f - 1.0f);
    if(!outline_gradient) mix_factor = 1.0f;
    for (std::int32_t sc = 1; sc < step_count; sc++)
      if (this_pos >= sc / fstep_count - outline_step_gap_factor * 0.5f
        && this_pos <= sc / fstep_count + outline_step_gap_factor * 0.5f)
        goto next_conical;
    g.setColour(active_outline_low.interpolatedWith(active_outline_high, mix_factor));
    g.strokePath(active_outline, PathStrokeType(outline_thickness));
next_conical:;
  }

  // highlight gradient
  float const highlight_offset = (outer_size - highlight_size) * 0.5f;
  float const hl_x = outer_x + highlight_offset;
  float const hl_y = outer_y + highlight_offset;
  Rectangle<float> hl_rect(hl_x, hl_y, highlight_size, highlight_size);
  fill_gradient_circle(g, s, hl_rect, colors::knob_highlight_low, colors::knob_highlight_high);

  // cuts
  float const cut_line_thickness = outer_size * cut_line_thickness_factor;
  float const cut_radius_outer = outer_radius * highlight_size_factor;
  float const cut_radius_inner = cut_radius_outer * center_size_factor * cut_inner_size_factor;
  float const cut_size = cut_radius_outer - cut_radius_inner;
  float const cut_top_left_x = center_x + cut_radius_outer * std::cos(0.625f * 2.0f * pi32);
  float const cut_top_left_y = center_y + cut_radius_outer * std::sin(0.625f * 2.0f * pi32);
  float const cut_bottom_right_x = center_x + cut_radius_outer * std::cos(0.125f * 2.0f * pi32);
  float const cut_bottom_right_y = center_y + cut_radius_outer * std::sin(0.125f * 2.0f * pi32);
  juce::Point<float> const cut_top_left(cut_top_left_x, cut_top_left_y);
  juce::Point<float> const cut_bottom_right(cut_bottom_right_x, cut_bottom_right_y);
  float const cut_max_distance = cut_top_left.getDistanceFrom(cut_bottom_right);
  for (std::int32_t i = 1; i < cut_count; i++) // 0 = thumb
  {
    Path cut;
    float const angle_part = angle + i * pi32 * 2.0f / cut_count;
    float const cut_end_x = center_x + cut_radius_outer * std::cos(angle_part);
    float const cut_end_y = center_y + cut_radius_outer * std::sin(angle_part);
    float const cut_start_x = center_x + cut_radius_inner * std::cos(angle_part);
    float const cut_start_y = center_y + cut_radius_inner * std::sin(angle_part);
    cut.addArrow(Line<float>(cut_start_x, cut_start_y, cut_end_x, cut_end_y), cut_line_thickness, cut_size * cut_xy_ratio, cut_size);
    auto const cut_inward_low = with_enabled(s, colors::knob_cuts_inward_low);
    auto const cut_outward_low = with_enabled(s, colors::knob_cuts_outward_low);
    auto const cut_inward_high = with_enabled(s, colors::knob_cuts_inward_high);
    auto const cut_outward_high = with_enabled(s, colors::knob_cuts_outward_high);
    juce::Point<float> const this_cut_pos(cut_end_x, cut_end_y);
    float const cut_mix_factor = this_cut_pos.getDistanceFrom(cut_top_left) / cut_max_distance;
    auto const cut_inward_mix = cut_inward_high.interpolatedWith(cut_inward_low, cut_mix_factor);
    auto const cut_outward_mix = cut_outward_high.interpolatedWith(cut_outward_low, cut_mix_factor);
    g.setGradientFill(ColourGradient(cut_inward_mix, cut_start_x, cut_start_y, cut_outward_mix, cut_end_x, cut_end_y, false));
    g.fillPath(cut);
  }

  // gradient fill center
  float const fill_offset = (highlight_size - center_size) * 0.5f;
  float const fill_x = hl_x + fill_offset;
  float const fill_y = hl_y + fill_offset;
  Rectangle<float> fill_rect(fill_x, fill_y, center_size, center_size);
  fill_gradient_circle(g, s, fill_rect, colors::knob_gradient_fill_low, colors::knob_gradient_fill_high);

  // thumb
  float const thumb_end_x = center_x + cut_radius_outer * std::cos(angle);
  float const thumb_end_y = center_y + cut_radius_outer * std::sin(angle);
  juce::Line<float> thumb_line(center_x, center_y, thumb_end_x, thumb_end_y);
  auto const thumb_inward_low = with_enabled(s, colors::knob_thumb_inward_low);
  auto const thumb_outward_low = with_enabled(s, colors::knob_thumb_outward_low);
  auto const thumb_inward_high = with_enabled(s, colors::knob_thumb_inward_high);
  auto const thumb_outward_high = with_enabled(s, colors::knob_thumb_outward_high);
  juce::Point<float> const thumb_pos(thumb_end_x, thumb_end_y);
  float const thumb_mix_factor = thumb_pos.getDistanceFrom(cut_top_left) / cut_max_distance;
  auto const thumb_inward_mix = thumb_inward_high.interpolatedWith(thumb_inward_low, thumb_mix_factor);
  auto const thumb_outward_mix = thumb_outward_high.interpolatedWith(thumb_outward_low, thumb_mix_factor);
  g.setGradientFill(ColourGradient(thumb_inward_mix, center_x, center_y, thumb_outward_mix, thumb_end_x, thumb_end_y, false));
  g.drawArrow(thumb_line, 0, thumb_width, cut_radius_outer);

  fill_spot_circle(g, s, fill_rect, colors::knob_spot_fill_low, colors::knob_spot_fill_high);

  // stroke center
  float const offset_radians = 0.25f * 2.0f * pi32;
  float const gap_radians = 1.0f / cut_count * 2.0f * pi32;
  float const center_radius = center_size * 0.5f;
  float const center_thickness = outer_size * center_thickness_factor;
  for (std::int32_t i = 0; i < fake_conic_gradient_count; i++)
  {
    Path stroke;
    float fi = static_cast<float>(i);
    float stroke_start_angle = (0.375f + fi / fake_conic_gradient_count) * 2.0f * pi32;
    float stroke_end_angle = (0.375f + (fi + 1.0f) / fake_conic_gradient_count) * 2.0f * pi32;
    if((stroke_start_angle - offset_radians >= angle - gap_radians * 0.5f &&
       stroke_end_angle - offset_radians <= angle + gap_radians * 0.5f) ||
       (stroke_start_angle - offset_radians >= angle - 2.0f * pi32 - gap_radians * 0.5f &&
       stroke_end_angle - offset_radians <= angle - 2.0f * pi32 + gap_radians * 0.5f)) continue;
    stroke.addCentredArc(center_x, center_y, center_radius, center_radius, 0.0f, stroke_start_angle, stroke_end_angle, true);
    auto stroke_low = with_enabled(s, colors::knob_center_stroke_low);
    auto stroke_high = with_enabled(s, colors::knob_center_stroke_high);
    float mix_factor = 1.0f - std::abs((i + 1.0f) / (fake_conic_gradient_count / 2.0f) - 1.0f);
    g.setColour(stroke_low.interpolatedWith(stroke_high, mix_factor));
    g.strokePath(stroke, PathStrokeType(center_thickness));
  }
}

} // namespace inf::base::ui