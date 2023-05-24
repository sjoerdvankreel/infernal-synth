#include <inf.base.ui/controls/selector_bar.hpp>

using namespace juce;

namespace inf::base::ui {

void 
inf_selector_bar::popupMenuClickOnTab(
  int index, juce::String const& name)
{
  PopupMenu menu;
  menu.addItem(1, "Clear");
  
  PopupMenu copy;
  for(std::int32_t i = 0; i < getNumTabs(); i++)
    copy.addItem(1 + i + 1, std::to_string(i + 1));
  menu.addSubMenu("Copy to", copy);
  
  PopupMenu swap;
  for (std::int32_t i = 0; i < getNumTabs(); i++)
    swap.addItem(1 + getNumTabs() + i + 1, std::to_string(i + 1));
  menu.addSubMenu("Swap with", swap);
  menu.setLookAndFeel(&getLookAndFeel());
  
  part_id this_id = { _part_type, getCurrentTabIndex() };  
  int option = menu.show();

  if(option == 0) 
    return;
  option--;
  if(option == 0)
  {
    _controller->clear_part(this_id);
    return;
  }
  option--;
  if(option < getNumTabs()) 
  {
    _controller->copy_or_swap_part(this_id, option, false);
    return;
  }
  option -= getNumTabs();
  assert(0 <= option && option < getNumTabs());
  _controller->copy_or_swap_part(this_id, option, true);
}

} // namespace inf::base::ui