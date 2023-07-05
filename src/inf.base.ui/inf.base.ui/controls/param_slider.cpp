#include <inf.base.ui/controls/param_slider.hpp>

using namespace juce;

namespace inf::base::ui {

void 
inf_param_slider::mouseDown(juce::MouseEvent const& e)
{
  if(e.mods.isLeftButtonDown())
    Slider::mouseDown(e);
}

void 
inf_param_slider::mouseUp(juce::MouseEvent const& e)
{ 
  if (e.mods.isLeftButtonDown())
    Slider::mouseUp(e);
  else if(e.mods.isRightButtonDown())
    show_host_menu_for_param(_controller, _param_index, &getLookAndFeel());
}

} // namespace inf::base::ui