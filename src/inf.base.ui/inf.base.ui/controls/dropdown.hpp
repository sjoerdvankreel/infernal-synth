#ifndef INF_BASE_UI_CONTROLS_DROPDOWN_HPP
#define INF_BASE_UI_CONTROLS_DROPDOWN_HPP

#include <inf.base.ui/shared/support.hpp>
#include <inf.base/topology/param_descriptor.hpp>
#include <juce_gui_basics/juce_gui_basics.h>
#include <cstdint>

namespace inf::base::ui {
  
class inf_dropdown:
public juce::ComboBox
{
private:
  base::param_descriptor const* _descriptor;
public:
  base::param_descriptor const* descriptor() const { return _descriptor; }
  inf_dropdown(base::param_descriptor const* descriptor):
  _descriptor(descriptor) {}
};

} // namespace inf::base::ui
#endif // INF_BASE_UI_CONTROLS_DROPDOWN_HPP