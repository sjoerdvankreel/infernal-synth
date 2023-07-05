#include <inf.base.ui/controls/button.hpp>
#include <inf.base.ui/shared/config.hpp>
#include <inf.base.ui/shared/look_and_feel.hpp>

using namespace juce;

namespace inf::base::ui {

void 
inf_button::paint(juce::Graphics& g)
{
  float const corner_size_fixed = 5.0f;
  float const outline_size_fixed = 1.0f;
  auto const bounds = getLocalBounds().toFloat().expanded(-button_margin);
  auto& lnf = dynamic_cast<inf_look_and_feel&>(getLookAndFeel());

  std::int32_t background_low = inf_look_and_feel::colors::button_background_low;
  std::int32_t background_high = inf_look_and_feel::colors::button_background_high;
  if (getState() == buttonOver)
  {
    background_low = inf_look_and_feel::colors::button_over_background_low;
    background_high = inf_look_and_feel::colors::button_over_background_high;
  }
  if (getState() == buttonDown)
  {
    background_low = inf_look_and_feel::colors::button_down_background_low;
    background_high = inf_look_and_feel::colors::button_down_background_high;
  }

  lnf.fill_gradient_rounded_rectangle(g, *this, bounds, 
    background_low, background_high, corner_size_fixed, 0.25f);
  lnf.stroke_gradient_rounded_rectangle(g, *this, bounds,
    inf_look_and_feel::colors::button_outline_low,
    inf_look_and_feel::colors::button_outline_high, corner_size_fixed, 0.25f, outline_size_fixed);
  g.setColour(findColour(inf_look_and_feel::colors::button_text));
  g.setFont(juce::Font(get_button_label_font_height(_controller), juce::Font::bold));
  g.drawText(getButtonText(), bounds, Justification::centred, false);
}

} // namespace inf::base::ui