#include <inf.base/shared/support.hpp>
#include <inf.base.ui/controls/icon.hpp>
#include <inf.base.ui/shared/look_and_feel.hpp>

using namespace inf::base;
using namespace juce;

namespace inf::base::ui {

void 
inf_icon::paint(Graphics& g)
{
  float const line_size = 2.0f;
  float const vpad_top = 1.0f;
  float const vpad_bottom = 7.0f;
  float const width_factor = 2.0f;
  float const y = vpad_top;
  float const h = getLocalBounds().getHeight() - vpad_bottom;
  float const w = width_factor * h;
  float const x = getLocalBounds().getX() + (getLocalBounds().getWidth() - w) * 0.5f;

  Path p;
  float point;
  std::int32_t w0 = static_cast<std::int32_t>(w);
  p.startNewSubPath(x, y + h / 2.0f);
  for (std::int32_t i = 0; i < w0; i++)
  {
    float fi = static_cast<float>(i);
    float ramp = fi/w;
    switch (_type)
    {
    case icon_type::pulse: point = std::round(ramp); break;
    case icon_type::saw: point = ramp < 0.5f? ramp + 0.5f: ramp - 0.5f; break;
    case icon_type::sine: point = (std::sin(2.0f * base::pi32 * ramp) + 1.0f) * 0.5f; break;
    default: point = ramp; break;
    }
    p.lineTo(x + fi, y + h - point * h);
  }
  p.lineTo(x + w0 - 1, y + h / 2.0f);
  g.setColour(findColour(inf_look_and_feel::colors::icon_stroke_color));
  g.strokePath(p, PathStrokeType(line_size));
}

} // namespace inf::base::ui