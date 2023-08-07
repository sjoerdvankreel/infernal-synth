#include <inf.base.ui/controls/param_dropdown.hpp>
#include <inf.base.ui/shared/ui.hpp>

using namespace juce;

namespace inf::base::ui {
  
void 
inf_param_dropdown::mouseUp(juce::MouseEvent const& e)
{
  ComboBox::mouseUp(e);
  if (!e.mods.isRightButtonDown()) return;
  show_context_menu_for_param(_controller, _param_index, false, &getLookAndFeel(), _lnf_factory);
}

} // namespace inf::base::ui