#include <inf.base.ui/controls/param_dropdown.hpp>
#include <inf.base.ui/shared/ui.hpp>

using namespace juce;

namespace inf::base::ui {

void
inf_param_dropdown::showPopup()
{
  update_items_enabled(getRootMenu());
  ComboBox::showPopup();
}
  
void 
inf_param_dropdown::mouseUp(juce::MouseEvent const& e)
{
  ComboBox::mouseUp(e);
  if (!e.mods.isRightButtonDown()) return;
  show_context_menu_for_param(_controller, _param_index, false, &getLookAndFeel(), _lnf_factory);
}

void 
inf_param_dropdown::update_items_enabled(juce::PopupMenu* menu)
{
  if (_item_enabled_selector == nullptr) return;
  PopupMenu::MenuItemIterator iter(*menu);
  while (iter.next())
    if (iter.getItem().subMenu.get())
      update_items_enabled(iter.getItem().subMenu.get());
    else
      iter.getItem().setEnabled(_item_enabled_selector(_controller, *_items_by_id.at(iter.getItem().itemID)));
}

} // namespace inf::base::ui