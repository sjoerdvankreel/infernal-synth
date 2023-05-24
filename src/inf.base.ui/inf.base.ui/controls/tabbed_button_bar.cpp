#include <inf.base.ui/controls/tabbed_button_bar.hpp>

using namespace juce;

namespace inf::base::ui {

void 
inf_tabbed_button_bar::popupMenuClickOnTab(
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
  menu.show();
}

} // namespace inf::base::ui