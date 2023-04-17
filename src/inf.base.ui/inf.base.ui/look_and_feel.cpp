#include <inf.base/shared/support.hpp>
#include <inf.base.ui/look_and_feel.hpp>

using namespace juce;

namespace inf::base::ui {

void 
inf_look_and_feel::drawRotarySlider(
  juce::Graphics& g, int x, int y, int w, int h, 
  float pos, float start, float end, juce::Slider& s)
{
  std::int32_t const cut_count = 12;

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

  // highlight cuts
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
  float knob_fill_size = highlight_size_factor * inner_size;
  float fillx = hlx + (1.0f - highlight_size_factor) * 0.5f * inner_size;
  float filly = hly + (1.0f - highlight_size_factor) * 0.5f * inner_size;
  auto fill_base = s.findColour(colors::knob_fill_base);
  auto fill_highlight = s.findColour(colors::knob_fill_highlight);
  auto fill_gradient = ColourGradient(fill_highlight, cx, cy, fill_base, fillx + knob_fill_size, filly + knob_fill_size, true);
  fill_gradient.addColour(0.25, fill_highlight.interpolatedWith(fill_base, 0.5f));
  g.setGradientFill(fill_gradient);
  g.fillEllipse(fillx, filly, knob_fill_size, knob_fill_size);

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
}

} // namespace inf::base::ui