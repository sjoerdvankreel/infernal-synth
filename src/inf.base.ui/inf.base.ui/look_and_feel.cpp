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
  std::int32_t const cut_count = 12;
  // std::int32_t const fill_round_count = 10;

  // config
  float const cut_xy_ratio = 1.25f;
  float const spot_size_factor = 0.67f;
  float const center_size_factor = 0.75f;
  float const cut_inner_size_factor = 0.85f;
  float const highlight_size_factor = 0.75f;
  float const center_thickness_factor = 0.0125f;
  float const outline_thickness_factor = 0.075f;
  float const cut_line_thickness_factor = 0.0125f;

  // adjust for nonrectangular
  float left = static_cast<float>(x);
  float top = static_cast<float>(y);
  if (w < h) top += (h - w) * 0.5f;
  if (h < w) left += (w - h) * 0.5f;

  // precompute stuff
  float const margin_factor = 0.05f;
  float const margin = std::min(w, h) * margin_factor;
  float const outer_size = std::min(w, h) - 2.0f * margin;
  float const highlight_size = outer_size * highlight_size_factor;
  float const center_size = highlight_size * center_size_factor;
  float const angle = start_angle + pos * (end_angle - start_angle) - pi32 * 0.5f;

  // precompute bounds
  float const outer_y = top + margin;
  float const outer_x = left + margin;
  float const center_y = top + margin + outer_size / 2.0f;
  float const center_x = left + margin + outer_size / 2.0f;

  // outline
  Path outline;
  float const outer_radius = outer_size / 2.0f;
  float const outline_thickness = outer_size * outline_thickness_factor;
  outline.addCentredArc(center_x, center_y, outer_radius, outer_radius, 0.0f, start_angle, end_angle, true);
  g.setColour(s.findColour(colors::knob_outline));
  g.strokePath(outline, PathStrokeType(outline_thickness));

  // highlight gradient
  float const highlight_offset = (outer_size - highlight_size) * 0.5f;
  float const hl_x = outer_x + highlight_offset;
  float const hl_y = outer_y + highlight_offset;
  auto const hl_shadow = s.findColour(colors::knob_shadow);
  auto const hl_highlight = s.findColour(colors::knob_highlight);
  auto hl_gradient = ColourGradient(hl_highlight, hl_x, hl_y, hl_shadow, hl_x + highlight_size, hl_y + highlight_size, false);
  hl_gradient.addColour(0.25, hl_highlight.interpolatedWith(hl_shadow, 0.5f));
  g.setGradientFill(hl_gradient);
  g.fillEllipse(hl_x, hl_y, highlight_size, highlight_size);

  // cuts
  float const cut_line_thickness = outer_size * cut_line_thickness_factor;
  float const cut_radius_outer = outer_radius * highlight_size_factor;
  float const cut_radius_inner = cut_radius_outer * center_size_factor * cut_inner_size_factor;
  float const cut_size = cut_radius_outer - cut_radius_inner;
  for (std::int32_t i = 0; i < cut_count; i++)
  {
    Path cut;
    float const angle_part = angle + i * pi32 * 2.0f / cut_count;
    float const cut_end_x = center_x + cut_radius_outer * std::cos(angle_part);
    float const cut_end_y = center_y + cut_radius_outer * std::sin(angle_part);
    float const cut_start_x = center_x + cut_radius_inner * std::cos(angle_part);
    float const cut_start_y = center_y + cut_radius_inner * std::sin(angle_part);
    cut.addArrow(Line<float>(cut_start_x, cut_start_y, cut_end_x, cut_end_y), cut_line_thickness, cut_size * cut_xy_ratio, cut_size);
    auto const cut_inward = s.findColour(colors::knob_cuts_inward);
    auto const cut_outward = s.findColour(colors::knob_cuts_outward);
    g.setGradientFill(ColourGradient(cut_inward, cut_start_x, cut_start_y, cut_outward, cut_end_x, cut_end_y, false));
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
  auto rad_fill_base = s.findColour(colors::knob_radial_fill_base);
  auto rad_fill_highlight = s.findColour(colors::knob_radial_fill_highlight);
  auto rad_fill_gradient = ColourGradient(rad_fill_highlight, spot_center_x, spot_center_y, rad_fill_base, fill_x + spot_size, fill_y + spot_size, true);
  rad_fill_gradient.addColour(0.25, rad_fill_highlight.interpolatedWith(rad_fill_base, 0.5f));
  g.setGradientFill(rad_fill_gradient);
  g.fillEllipse(spot_x, spot_y, spot_size, spot_size);

  // stroke center
  float const center_thickness = outer_size * center_thickness_factor;
  center_thickness;
  g.setColour(s.findColour(colors::knob_cuts_inward));
  g.drawEllipse(fill_x, fill_y, center_size, center_size, center_thickness);

  /*
  // fill
  for (std::int32_t i = 0; i < fill_round_count; i++)
  {
    float const fill_size = center_size * (fill_round_count - i) / fill_round_count;
    float const fill_offset = (highlight_size - fill_size) * 0.5f;
    auto fill_base = s.findColour(colors::knob_fill_base);
    auto fill_highlight = s.findColour(colors::knob_fill_highlight);
    g.setColour(fill_base.interpolatedWith(fill_highlight, i / static_cast<float>(fill_round_count - 1)));
    g.fillEllipse(hl_x + fill_offset, hl_y + fill_offset, fill_size, fill_size);
  }
  */

/*
  std::int32_t const cut_count = 12;
  std::int32_t const fill_round_count = 6;

  // relative to min(w, h)
  float const margin_factor = 0.05f;
  float const inner_size_factor = 0.75f;
  float const cut_inner_size_factor = 0.9f;
  float const highlight_size_factor = 0.75f;
  float const cut_line_thickness_factor = 0.0125f;
  float const thumb_line_thickness_factor = 0.025f;
  float const outline_line_thickness_factor = 0.075f;

  // adjust for nonrectangular
  float rx = static_cast<float>(x);
  float ry = static_cast<float>(y);
  if(h < w) rx += (w - h) * 0.5f;
  if(w < h) ry += (h - w) * 0.5f;

  // precompute stuff
  float const margin = std::min(w, h) * margin_factor;
  float const size = std::min(w, h) - 2.0f * margin;
  float const radius = size / 2.0f;
  float const inner_size = size * inner_size_factor;
  float const angle = start + pos * (end - start) - pi32 * 0.5f;
  float const fx = static_cast<float>(rx + margin);
  float const fy = static_cast<float>(ry + margin);
  float const cut_line_thickness = size * cut_line_thickness_factor;
  float const thumb_line_thickness = size * thumb_line_thickness_factor;
  float const outline_line_thickness = size * outline_line_thickness_factor;

  // highlight
  float hlx = fx + (size - inner_size) * 0.5f;
  float hly = fy + (size - inner_size) * 0.5f;
  auto shadow = s.findColour(colors::knob_shadow);
  auto high = s.findColour(colors::knob_highlight);
  auto hl_gradient = ColourGradient(high, hlx, hly, shadow, hlx + inner_size, hly + inner_size, false);
  hl_gradient.addColour(0.25, high.interpolatedWith(shadow, 0.5f));
  g.setGradientFill(hl_gradient);
  g.fillEllipse(hlx, hly, inner_size, inner_size);

  // cuts
  float cx = rx + margin + size / 2.0f;
  float cy = ry + margin + size / 2.0f;
  float hl_cut_radius_outer = radius * inner_size_factor;
  float hl_cut_radius_inner = radius * inner_size_factor * highlight_size_factor * cut_inner_size_factor;
  float cut_size = hl_cut_radius_outer - hl_cut_radius_inner;
  for (std::int32_t i = 0; i < cut_count; i++)
  {
    Path cut;    
    float cut_end_x = cx + hl_cut_radius_outer * std::cos(angle + i * pi32 * 2.0f / cut_count);
    float cut_end_y = cy + hl_cut_radius_outer * std::sin(angle + i * pi32 * 2.0f / cut_count);
    float cut_start_x = cx + hl_cut_radius_inner * std::cos(angle + i * pi32 * 2.0f / cut_count);
    float cut_start_y = cy + hl_cut_radius_inner * std::sin(angle + i * pi32 * 2.0f / cut_count);
    cut.addArrow(Line<float>(cut_start_x, cut_start_y, cut_end_x, cut_end_y), cut_line_thickness, cut_size, cut_size);
    auto cut_inward = s.findColour(colors::knob_cuts_inward);
    auto cut_outward = s.findColour(colors::knob_cuts_outward);
    g.setGradientFill(ColourGradient(cut_inward, cut_start_x, cut_start_y, cut_outward, cut_end_x, cut_end_y, false));
    g.fillPath(cut);
  }
  
  // fill
  float ffill_round_count = static_cast<float>(fill_round_count);
  for(std::int32_t i = 0; i < fill_round_count; i++)
  {
    float knob_fill_size = highlight_size_factor * inner_size * ((ffill_round_count - i) / ffill_round_count);
    float fillx = hlx + (1.0f - highlight_size_factor) * 0.5f * inner_size;
    float filly = hly + (1.0f - highlight_size_factor) * 0.5f * inner_size;
    auto fill_base = s.findColour(colors::knob_fill_base);
    auto fill_highlight = s.findColour(colors::knob_fill_highlight);
    g.setColour(fill_base.interpolatedWith(fill_highlight, i / (ffill_round_count - 1.0f)));
    g.fillEllipse(fillx, filly, knob_fill_size, knob_fill_size);
  }

  // thumb
  Path thumb;
  float thumb_radius = radius * inner_size_factor * highlight_size_factor;
  float thumb_end_x = cx + thumb_radius * std::cos(angle);
  float thumb_end_y = cy + thumb_radius * std::sin(angle);
  thumb.addLineSegment(Line<float>(cx, cy, thumb_end_x, thumb_end_y), 1.0f);
  g.setColour(s.findColour(colors::knob_thumb));
  g.strokePath(thumb, PathStrokeType(thumb_line_thickness));

  // outline
  Path arc;
  arc.addCentredArc(cx, cy, radius, radius, 0.0f, start, end, true);
  g.setColour(s.findColour(colors::knob_outline));
  g.strokePath(arc, PathStrokeType(outline_line_thickness));
  */
}

} // namespace inf::base::ui