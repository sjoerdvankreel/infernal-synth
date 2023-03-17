#include <inf.base.ui/shared/support.hpp>
#include <inf.base.ui/controls/nested_option_menu.hpp>
#include <vstgui/uidescription/uiviewcreator.h>

using namespace inf::base;
using namespace VSTGUI;
using namespace VSTGUI::UIViewCreator;

namespace inf::base::ui {

static void 
add_item(COptionMenu* menu, std::string const* path, std::size_t path_count)
{
  assert(path != nullptr && path_count > 0);
  CMenuItem* item = nullptr;
  for(std::size_t i = 0; i < menu->getItems()->size(); i++)
    if ((*path) == menu->getItems()->at(i)->getTitle())
    {
      item = menu->getItems()->at(i).get();
      break;
    }
  if(item == nullptr)
    item = menu->addEntry(path[0].c_str());
  if(path_count > 1)
  {
    if (item->getSubmenu() == nullptr)
      item->setSubmenu(makeOwned<COptionMenu>());
    add_item(item->getSubmenu(), path + 1, path_count - 1);
  }
}
 
nested_option_menu::
~nested_option_menu()
{ unregisterOptionMenuListener(this); }

nested_option_menu::
nested_option_menu(): COptionMenu()
{ registerOptionMenuListener(this); }
  
CView* 
nested_option_menu_creator::create(
  UIAttributes const& attrs, IUIDescription const* desc) const
{ return new nested_option_menu(); }
 
bool 
nested_option_menu_creator::apply(
  CView* view, UIAttributes const& attrs, IUIDescription const* desc) const
{ return OptionMenuCreator::apply(view, attrs, desc); }

// Swap flat menu for nested menu.
void 
nested_option_menu::onOptionMenuPrePopup(COptionMenu* menu)
{
  assert(menu == this);
  beginEdit();
  _flat_list = *menuItems;
  menuItems->clear();
  topology_info const* topology = find_editor(this)->topology();
  auto index = topology->param_id_to_index.at(getTag());
  auto const& param = topology->params.at(index);
  auto items = param.descriptor->data.discrete.items;
  for(std::size_t i = 0; i < items->size(); i++)
    add_item(menu, items->at(i).submenu_path.data(), items->at(i).submenu_path.size());
}

// Swap tree menu for flat, and select index in flat list.
void 
nested_option_menu::onOptionMenuPostPopup(COptionMenu* menu)
{
  menuItems->clear();
  std::int32_t index = -1;
  for(std::size_t i = 0; i < _flat_list.size(); i++)
  {
    menuItems->push_back(_flat_list[i]);
    if (_selected_item == _flat_list[i]->getTitle())
      index = static_cast<std::int32_t>(i);
  }
  _flat_list.clear();
  if(index != -1) 
  {
    setCurrent(index);
    setValue(static_cast<float>(index));
    valueChanged();
  }
  _selected_item.clear();
  endEdit();
}

// Remember selected text (should be unique through the entire tree).
bool 
nested_option_menu::onOptionMenuSetPopupResult(COptionMenu* menu, COptionMenu* selected, std::int32_t index)
{
  _selected_item = selected->getItems()->at(index)->getTitle();
  return true; // Prevent set value, need to restore flat list first.
}

} // namespace inf::base::ui