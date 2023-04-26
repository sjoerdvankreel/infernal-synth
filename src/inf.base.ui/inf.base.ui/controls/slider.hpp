#ifndef INF_BASE_UI_CONTROLS_SLIDER_HPP
#define INF_BASE_UI_CONTROLS_SLIDER_HPP

#include <inf.base/topology/param_descriptor.hpp>
#include <juce_gui_basics/juce_gui_basics.h>
#include <cstdint>

namespace inf::base::ui {

enum class slider_kind { knob, selector, hslider, vslider }; 
  
class inf_slider:
public juce::Slider
{
private:
  slider_kind _kind;
  base::param_descriptor const* _descriptor;
public:
  slider_kind kind() const { return _kind; }
  base::param_descriptor const* descriptor() const { return _descriptor; }
  inf_slider(base::param_descriptor const* descriptor, slider_kind kind):
  _kind(kind), _descriptor(descriptor) {}
};

} // namespace inf::base::ui
#endif // INF_BASE_UI_CONTROLS_SLIDER_HPP