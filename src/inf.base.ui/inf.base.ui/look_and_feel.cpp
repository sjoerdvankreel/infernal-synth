#include <inf.base.ui/look_and_feel.hpp>

namespace inf::base::ui {

void 
inf_look_and_feel::drawRotarySlider(
  juce::Graphics& g, int x, int y, int w, int h, 
  float pos, float start, float end, juce::Slider& s)
{
  LookAndFeel_V4::drawRotarySlider(g, x, y, w, h, pos, start, end, s);
}

} // namespace inf::base::ui