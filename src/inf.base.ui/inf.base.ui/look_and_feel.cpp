#include <inf.base/shared/support.hpp>
#include <inf.base.ui/look_and_feel.hpp>

using namespace juce;

namespace inf::base::ui {

void 
inf_look_and_feel::drawRotarySlider(
  juce::Graphics& g, int x, int y, int w, int h, 
  float pos, float start, float end, juce::Slider& s)
{
  float const radius = w / 2.0f;
  float const line_thickness = 2.0f;
  float const outline_thickness = 2.0f;
  float const angle = pos * 2.0f * pi32;

  float fx = static_cast<float>(x);
  float fy = static_cast<float>(y);
  float fw = static_cast<float>(w);
  float fh = static_cast<float>(h);
  auto fill = s.findColour(Slider::ColourIds::rotarySliderFillColourId);
  g.setColour(fill);
  g.fillEllipse(fx, fy, fw, fh);
  auto outline = s.findColour(Slider::ColourIds::rotarySliderOutlineColourId);
  g.setColour(outline);
  g.drawEllipse(fx, fy, fw, fh, outline_thickness);
  auto thumb = s.findColour(Slider::ColourIds::thumbColourId);
  g.setColour(thumb);
  float cx = w / 2.0f;
  float cy = h / 2.0f;
  float line_end_x = cx + radius * std::cos(angle);
  float line_end_y = cy + radius * std::sin(angle);
  g.drawLine(cx, cy, line_end_x, line_end_y, line_thickness);
  
  //LookAndFeel_V4::drawRotarySlider(g, x, y, w, h, pos, start, end, s);

  /*
  
   auto outline = slider.findColour (Slider::rotarySliderOutlineColourId);
    auto fill    = slider.findColour (Slider::rotarySliderFillColourId);

    auto bounds = Rectangle<int> (x, y, width, height).toFloat().reduced (10);

    auto radius = jmin (bounds.getWidth(), bounds.getHeight()) / 2.0f;
    auto toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    auto lineW = jmin (8.0f, radius * 0.5f);
    auto arcRadius = radius - lineW * 0.5f;

    Path backgroundArc;
    backgroundArc.addCentredArc (bounds.getCentreX(),
                                 bounds.getCentreY(),
                                 arcRadius,
                                 arcRadius,
                                 0.0f,
                                 rotaryStartAngle,
                                 rotaryEndAngle,
                                 true);

    g.setColour (outline);
    g.strokePath (backgroundArc, PathStrokeType (lineW, PathStrokeType::curved, PathStrokeType::rounded));

    if (slider.isEnabled())
    {
        Path valueArc;
        valueArc.addCentredArc (bounds.getCentreX(),
                                bounds.getCentreY(),
                                arcRadius,
                                arcRadius,
                                0.0f,
                                rotaryStartAngle,
                                toAngle,
                                true);

        g.setColour (fill);
        g.strokePath (valueArc, PathStrokeType (lineW, PathStrokeType::curved, PathStrokeType::rounded));
    }

    auto thumbWidth = lineW * 2.0f;
    Point<float> thumbPoint (bounds.getCentreX() + arcRadius * std::cos (toAngle - MathConstants<float>::halfPi),
                             bounds.getCentreY() + arcRadius * std::sin (toAngle - MathConstants<float>::halfPi));

    g.setColour (slider.findColour (Slider::thumbColourId));
    g.fillEllipse (Rectangle<float> (thumbWidth, thumbWidth).withCentre (thumbPoint));*/
}

} // namespace inf::base::ui