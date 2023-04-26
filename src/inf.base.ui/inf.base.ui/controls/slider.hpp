#ifndef INF_BASE_UI_CONTROLS_SLIDER_HPP
#define INF_BASE_UI_CONTROLS_SLIDER_HPP

#include <inf.base/topology/param_descriptor.hpp>
#include <juce_gui_basics/juce_gui_basics.h>
#include <cstdint>

namespace inf::base::ui {

class inf_slider:
public juce::Slider
{
private:
  bool const _outline_gradient;
  base::param_descriptor const* _descriptor;
public:
  bool outline_gradient() const { return _outline_gradient; }
  base::param_descriptor const* descriptor() const { return _descriptor; }
  inf_slider(base::param_descriptor const* descriptor, bool outline_gradient): 
  _outline_gradient(outline_gradient), _descriptor(descriptor) {}
};

} // namespace inf::base::ui
#endif // INF_BASE_UI_CONTROLS_SLIDER_HPP