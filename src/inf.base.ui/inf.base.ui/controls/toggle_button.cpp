#include <inf.base.ui/controls/toggle_button.hpp>
#include <inf.base.ui/shared/ui.hpp>

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
    show_context_menu_for_param(_controller, _param_index, false, &getLookAndFeel(), _lnf_factory);
}

} // namespace inf::base::ui