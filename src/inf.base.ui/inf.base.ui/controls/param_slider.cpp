#include <inf.base.ui/controls/param_slider.hpp>
#include <inf.base.ui/shared/ui.hpp>

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
    show_context_menu_for_param(_controller, _param_index, isEnabled(), &getLookAndFeel(), _lnf_factory);
}

} // namespace inf::base::ui