#include <inf.base/shared/support.hpp>
#include <inf.base.ui/shared/look_and_feel.hpp>
#include <inf.base.ui/controls/slider.hpp>

using namespace juce;

namespace inf::base::ui {

void 
inf_look_and_feel::drawLabel(Graphics& g, Label& label)
{
  auto const& font = getLabelFont(label);
  g.fillAll(label.findColour(Label::backgroundColourId));
  g.setFont(font);
  g.setColour(label.findColour(Label::textColourId));
  auto text_area = getLabelBorderSize(label).subtractedFrom(label.getLocalBounds());
  g.drawText(label.getText(), text_area, label.getJustificationType(), false);
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
  float const spot_size_factor = 0.67f;
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
  auto hl_low = s.findColour(colors::slider_highlight_low);
  auto hl_high = s.findColour(colors::slider_highlight_high);
  juce::Rectangle<float> track_rect(
    start_x - track_size / 2.0f, 
    start_y - track_size / 2.0f, 
    vertical ? track_size : w + track_size, 
    vertical ? h + track_size : track_size);
  auto hl_gradient = ColourGradient(hl_low, track_rect.getTopLeft(), hl_high, track_rect.getBottomRight(), false);
  hl_gradient.addColour(0.75, hl_high.interpolatedWith(hl_low, 0.5f));
  g.setGradientFill(hl_gradient);
  g.fillRoundedRectangle(track_rect, track_size / 2.0f);

  // track inactive fill
  float track_inner_size = track_size * track_inner_size_factor;
  float track_inner_offset = track_size - track_inner_size;
  juce::Rectangle<float> track_inner_rect(
    track_rect.getX() + track_inner_offset / 2.0f, 
    track_rect.getY() + track_inner_offset / 2.0f, 
    track_rect.getWidth() - track_inner_offset, 
    track_rect.getHeight() - track_inner_offset);
  g.setColour(s.findColour(colors::slider_track_inactive));
  g.fillRoundedRectangle(track_inner_rect, track_inner_size / 2.0f);

  // track active
  Path track_active;
  auto track_low = s.findColour(colors::slider_track_low);
  auto track_high = s.findColour(colors::slider_track_high);
  if(bipolar)
  {
    track_active.startNewSubPath(Point<float>(mid_x, mid_y));
    track_active.lineTo(Point<float>(pos_x, active_pos_y));
    float const gradient_x = pos >= 0.5f? end_x: start_x;
    float const gradient_y = vertical && pos >= 0.5f || !vertical && pos < 0.5f? active_start_y: active_end_y;
    g.setGradientFill(ColourGradient(track_low, mid_x, mid_y, track_high, gradient_x, gradient_y, false));
  } else
  {
    track_active.startNewSubPath(Point<float>(start_x, active_start_y));
    track_active.lineTo(Point<float>(pos_x, active_pos_y));
    g.setGradientFill(ColourGradient(track_low, start_x, active_start_y, track_high, end_x, active_end_y, false));
  }
  g.strokePath(track_active, { track_inner_size, PathStrokeType::curved, bipolar? PathStrokeType::butt: PathStrokeType::rounded });

  // thumb fill + gradient overlay
  (void)spot_size_factor;
  float const thumb_large = track_size * thumb_size_factor;
  float const thumb_small = thumb_large / thumb_xy_ratio;
  juce::Rectangle<float> thumb_rect(
    pos_x - 0.5f * (vertical? thumb_large: thumb_small), 
    pos_y - 0.5f * (vertical? thumb_small: thumb_large), 
    vertical? thumb_large: thumb_small, 
    vertical? thumb_small: thumb_large);
  g.setColour(s.findColour(colors::slider_center_fill));
  g.fillRoundedRectangle(thumb_rect, thumb_small / 2.0f);
  auto thumb_low = s.findColour(colors::slider_gradient_fill_base);
  auto thumb_high = s.findColour(colors::slider_gradient_fill_highlight);
  auto thumb_gradient = ColourGradient(thumb_high, thumb_rect.getTopLeft(), thumb_low, thumb_rect.getBottomRight(), false);
  thumb_gradient.addColour(0.25, thumb_high.interpolatedWith(thumb_low, 0.5f));
  g.setGradientFill(thumb_gradient);
  g.fillRoundedRectangle(thumb_rect, thumb_small / 2.0f);

  // thumb spot
  float const spot_size = spot_size_factor * thumb_small;
  float const spot_offset = (thumb_small - spot_size) * 0.5f;
  float const spot_x = thumb_rect.getX() + spot_offset * 0.5f;
  float const spot_y = thumb_rect.getY() + spot_offset * 0.5f;
  float const spot_center_x = spot_x + spot_size * 0.5f;
  float const spot_center_y = spot_y + spot_size * 0.5f;
  auto spot_fill_base = s.findColour(colors::slider_spot_fill_base);
  auto spot_fill_highlight = s.findColour(colors::slider_spot_fill_highlight);
  auto spot_fill_gradient = ColourGradient(spot_fill_highlight, spot_center_x, spot_center_y, spot_fill_base, spot_x + spot_size, spot_y + spot_size, true);
  spot_fill_gradient.addColour(0.25, spot_fill_highlight.interpolatedWith(spot_fill_base, 0.5f));
  g.setGradientFill(spot_fill_gradient);
  g.fillEllipse(spot_x, spot_y, spot_size, spot_size);
}

void 
inf_look_and_feel::drawRotarySlider(
  juce::Graphics& g, int x0, int y0, int w0, int h0, 
  float pos, float start_angle, float end_angle, juce::Slider& s)
{
  // config
  float const cut_xy_ratio = 1.0f;
  float const margin_factor = 0.075f;
  float const margin_fixed_px = 0.0f;
  float const spot_size_factor = 0.67f;
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
  if (s.getInterval() > 0.0) step_count = static_cast<std::int32_t>(std::round(s.getMaximum() - s.getMinimum()));

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
  g.setColour(s.findColour(colors::knob_outline_inactive));
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
    auto active_outline_low = s.findColour(colors::knob_outline_low);
    auto active_outline_high = s.findColour(colors::knob_outline_high);
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
  auto const hl_low = s.findColour(colors::knob_highlight_low);
  auto const hl_high = s.findColour(colors::knob_highlight_high);
  auto hl_gradient = ColourGradient(hl_high, hl_x, hl_y, hl_low, hl_x + highlight_size, hl_y + highlight_size, false);
  hl_gradient.addColour(0.25, hl_high.interpolatedWith(hl_low, 0.5f));
  g.setGradientFill(hl_gradient);
  g.fillEllipse(hl_x, hl_y, highlight_size, highlight_size);

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
    auto const cut_inward_low = s.findColour(colors::knob_cuts_inward_low);
    auto const cut_outward_low = s.findColour(colors::knob_cuts_outward_low);
    auto const cut_inward_high = s.findColour(colors::knob_cuts_inward_high);
    auto const cut_outward_high = s.findColour(colors::knob_cuts_outward_high);
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
  auto grad_fill_base = s.findColour(colors::knob_gradient_fill_base);
  auto grad_fill_highlight = s.findColour(colors::knob_gradient_fill_highlight);
  auto grad_fill_gradient = ColourGradient(grad_fill_highlight, fill_x, fill_y, grad_fill_base, fill_x + center_size, fill_y + center_size, false);
  grad_fill_gradient.addColour(0.33, grad_fill_highlight.interpolatedWith(grad_fill_base, 0.5f));
  g.setGradientFill(grad_fill_gradient);
  g.fillEllipse(fill_x, fill_y, center_size, center_size);

  // thumb
  float const thumb_end_x = center_x + cut_radius_outer * std::cos(angle);
  float const thumb_end_y = center_y + cut_radius_outer * std::sin(angle);
  juce::Line<float> thumb_line(center_x, center_y, thumb_end_x, thumb_end_y);
  auto const thumb_inward_low = s.findColour(colors::knob_thumb_inward_low);
  auto const thumb_outward_low = s.findColour(colors::knob_thumb_outward_low);
  auto const thumb_inward_high = s.findColour(colors::knob_thumb_inward_high);
  auto const thumb_outward_high = s.findColour(colors::knob_thumb_outward_high);
  juce::Point<float> const thumb_pos(thumb_end_x, thumb_end_y);
  float const thumb_mix_factor = thumb_pos.getDistanceFrom(cut_top_left) / cut_max_distance;
  auto const thumb_inward_mix = thumb_inward_high.interpolatedWith(thumb_inward_low, thumb_mix_factor);
  auto const thumb_outward_mix = thumb_outward_high.interpolatedWith(thumb_outward_low, thumb_mix_factor);
  g.setGradientFill(ColourGradient(thumb_inward_mix, center_x, center_y, thumb_outward_mix, thumb_end_x, thumb_end_y, false));
  g.drawArrow(thumb_line, 0, thumb_width, cut_radius_outer);

  // radial fill spot overlay
  float const spot_size = spot_size_factor * center_size;
  float const spot_offset = (center_size - spot_size) * 0.5f;
  float const spot_x = fill_x + spot_offset * 0.5f;
  float const spot_y = fill_y + spot_offset * 0.5f;
  float const spot_center_x = spot_x + spot_size * 0.5f;
  float const spot_center_y = spot_y + spot_size * 0.5f;
  auto spot_fill_base = s.findColour(colors::knob_spot_fill_base);
  auto spot_fill_highlight = s.findColour(colors::knob_spot_fill_highlight);
  auto spot_fill_gradient = ColourGradient(spot_fill_highlight, spot_center_x, spot_center_y, spot_fill_base, fill_x + spot_size, fill_y + spot_size, true);
  spot_fill_gradient.addColour(0.25, spot_fill_highlight.interpolatedWith(spot_fill_base, 0.5f));
  g.setGradientFill(spot_fill_gradient);
  g.fillEllipse(spot_x, spot_y, spot_size, spot_size);

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
    if(stroke_start_angle - offset_radians >= angle - gap_radians * 0.5f &&
       stroke_end_angle - offset_radians <= angle + gap_radians * 0.5f ||
       stroke_start_angle - offset_radians >= angle - 2.0f * pi32 - gap_radians * 0.5f &&
       stroke_end_angle - offset_radians <= angle - 2.0f * pi32 + gap_radians * 0.5f) continue;
    stroke.addCentredArc(center_x, center_y, center_radius, center_radius, 0.0f, stroke_start_angle, stroke_end_angle, true);
    auto stroke_low = s.findColour(colors::knob_center_stroke_low);
    auto stroke_high = s.findColour(colors::knob_center_stroke_high);
    float mix_factor = 1.0f - std::abs((i + 1.0f) / (fake_conic_gradient_count / 2.0f) - 1.0f);
    g.setColour(stroke_low.interpolatedWith(stroke_high, mix_factor));
    g.strokePath(stroke, PathStrokeType(center_thickness));
  }
}

} // namespace inf::base::ui