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

  if(option == 0) return;
  else if(option == 1) _controller->clear_part(this_id);
  else if(option < 1 + getNumTabs()) _controller->copy_or_swap_part(this_id, option - 2, false);
  else if(option < 1 + getNumTabs() * 2) _controller->copy_or_swap_part(this_id, option - 2 - getNumTabs(), true);
  else assert(false);
}

} // namespace inf::base::ui