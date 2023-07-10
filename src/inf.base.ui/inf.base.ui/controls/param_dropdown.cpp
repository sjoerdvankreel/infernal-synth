#include <inf.base.ui/controls/param_dropdown.hpp>

using namespace juce;

namespace inf::base::ui {
  
void 
inf_param_dropdown::mouseUp(juce::MouseEvent const& e)
{
  ComboBox::mouseUp(e);
  if (!e.mods.isRightButtonDown()) return;
  show_host_menu_for_param(_controller, _param_index, &getLookAndFeel());
}

} // namespace inf::base::ui