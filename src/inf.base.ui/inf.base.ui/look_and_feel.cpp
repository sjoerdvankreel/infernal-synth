#include <inf.base/shared/support.hpp>
#include <inf.base.ui/look_and_feel.hpp>

using namespace juce;

namespace inf::base::ui {

void 
inf_look_and_feel::drawRotarySlider(
  juce::Graphics& g, int x, int y, int w, int h, 
  float pos, float start_angle, float end_angle, juce::Slider& s)
{
  // config
  float const cut_xy_ratio = 1.0f;
  float const spot_size_factor = 0.67f;
  float const thumb_width_factor = 0.1f;
  float const center_size_factor = 0.67f;
  float const cut_inner_size_factor = 0.85f;
  float const highlight_size_factor = 0.75f;
  float const center_thickness_factor = 0.0125f;
  float const outline_thickness_factor = 0.075f;
  float const cut_line_thickness_factor = 0.0125f;

  // config
  std::int32_t const cut_count = 10;
  std::int32_t const fake_conic_gradient_count = 1024;

  // adjust for nonrectangular
  float left = static_cast<float>(x);
  float top = static_cast<float>(y);
  if (w < h) top += (h - w) * 0.5f;
  if (h < w) left += (w - h) * 0.5f;

  // precompute stuff
  float const margin_factor = 0.05f;
  float const margin = std::min(w, h) * margin_factor;
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
  Path inactive_outline;
  float const outer_radius = outer_size / 2.0f;
  float const outline_thickness = outer_size * outline_thickness_factor;
  inactive_outline.addCentredArc(center_x, center_y, outer_radius, outer_radius, 0.0f, start_angle, end_angle, true);
  g.setColour(s.findColour(colors::knob_outline_inactive));
  g.strokePath(inactive_outline, PathStrokeType(outline_thickness));

  // active outline
  for (std::int32_t i = 0; i < fake_conic_gradient_count; i++)
  {
    Path active_outline;
    float fi = static_cast<float>(i);
    if(fi / fake_conic_gradient_count >= pos) break;
    float active_start_angle = start_angle + fi / fake_conic_gradient_count * (end_angle - start_angle);
    float active_end_angle = start_angle + (fi + 1.0f) / fake_conic_gradient_count * (end_angle - start_angle);
    active_outline.addCentredArc(center_x, center_y, outer_radius, outer_radius, 0.0f, active_start_angle, active_end_angle, true);
    auto active_outline_low = s.findColour(colors::knob_outline_low);
    auto active_outline_high = s.findColour(colors::knob_outline_high);
    g.setColour(active_outline_low.interpolatedWith(active_outline_high, (i + 1.0f) / fake_conic_gradient_count));
    g.strokePath(active_outline, PathStrokeType(outline_thickness));
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

  // thumb
  float const thumb_radius_inner = cut_radius_inner - (cut_radius_outer - cut_radius_inner);
  float const thumb_end_x = center_x + cut_radius_outer * std::cos(angle);
  float const thumb_end_y = center_y + cut_radius_outer * std::sin(angle);
  float const thumb_start_x = center_x + thumb_radius_inner * std::cos(angle);
  float const thumb_start_y = center_y + thumb_radius_inner * std::sin(angle);
  juce::Line<float> thumb_line(thumb_start_x, thumb_start_y, thumb_end_x, thumb_end_y);
  g.setColour(Colours::red);
  g.drawArrow(thumb_line, 0, thumb_width, cut_radius_outer - thumb_radius_inner);

  // stroke center
  float const center_radius = center_size * 0.5f;
  float const center_thickness = outer_size * center_thickness_factor;
  for (std::int32_t i = 0; i < fake_conic_gradient_count / 2; i++)
    for(std::int32_t dir = -1; dir <= 1; dir += 2)
    {
      Path stroke;
      float fi = static_cast<float>(i);
      float thumb_angle_normal = angle / (2.0f * pi32);
      float thumb_end_angle_normal = thumb_angle_normal + 0.5f / cut_count;
      float thumb_start_angle_normal = thumb_angle_normal - 0.5f / cut_count;
      float stroke_start_angle_normal = 0.375f + dir * fi / fake_conic_gradient_count;
      float stroke_end_angle_normal = 0.375f + dir * (fi + 1.0f) / fake_conic_gradient_count;
      if (thumb_start_angle_normal <= stroke_start_angle_normal && stroke_start_angle_normal <= thumb_end_angle_normal) continue;
      float stroke_end_angle = stroke_end_angle_normal * 2.0f * pi32;
      float stroke_start_angle = stroke_start_angle_normal * 2.0f * pi32;
      stroke.addCentredArc(center_x, center_y, center_radius, center_radius, 0.0f, stroke_start_angle, stroke_end_angle, true);
      auto stroke_low = s.findColour(colors::knob_center_stroke_low);
      auto stroke_high = s.findColour(colors::knob_center_stroke_high);
      g.setColour(stroke_low.interpolatedWith(stroke_high, (i + 1.0f) / (fake_conic_gradient_count * 0.5f)));
      g.strokePath(stroke, PathStrokeType(center_thickness));
    }
}

} // namespace inf::base::ui