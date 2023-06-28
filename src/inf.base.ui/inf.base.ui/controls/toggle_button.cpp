#include <inf.base.ui/controls/toggle_button.hpp>

using namespace juce;

namespace inf::base::ui {
  
void 
inf_toggle_button::mouseUp(juce::MouseEvent const& e)
{
  ToggleButton::mouseUp(e);
  if (!e.mods.isRightButtonDown()) return;
  show_host_menu_for_param(_controller, _param_index, &getLookAndFeel());
}

} // namespace inf::base::ui