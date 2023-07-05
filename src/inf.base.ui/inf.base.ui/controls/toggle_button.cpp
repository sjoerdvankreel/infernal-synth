#include <inf.base.ui/controls/toggle_button.hpp>

using namespace juce;

namespace inf::base::ui {

void 
inf_toggle_button::mouseDown(juce::MouseEvent const& e)
{
  if(e.mods.isLeftButtonDown())
    ToggleButton::mouseDown(e);
}

void 
inf_toggle_button::mouseUp(juce::MouseEvent const& e)
{ 
  if (e.mods.isLeftButtonDown())
    ToggleButton::mouseUp(e);
  else if(e.mods.isRightButtonDown())
    show_host_menu_for_param(_controller, _param_index, &getLookAndFeel());
}

} // namespace inf::base::ui