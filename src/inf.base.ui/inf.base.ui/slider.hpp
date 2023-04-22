#ifndef INF_BASE_UI_SLIDER_HPP
#define INF_BASE_UI_SLIDER_HPP

#include <juce_gui_basics/juce_gui_basics.h>
#include <cstdint>

namespace inf::base::ui {

class inf_slider:
public juce::Slider
{
private:
  bool _bipolar;
public:
  bool bipolar() const { return _bipolar; }
  void bipolar(bool bipolar) { _bipolar = bipolar; }
};

} // namespace inf::base::ui
#endif // INF_BASE_UI_SLIDER_HPP