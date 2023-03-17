#ifndef INF_BASE_UI_CONTROLS_NESTED_OPTION_MENU_HPP
#define INF_BASE_UI_CONTROLS_NESTED_OPTION_MENU_HPP

#include <vstgui/vstgui.h>
#include <vstgui/vstgui_uidescription.h>
#include <vstgui/lib/controls/coptionmenu.h>
#include <vstgui/uidescription/viewcreator/optionmenucreator.h>
#include <vstgui/uidescription/detail/uiviewcreatorattributes.h>

namespace inf::base::ui {

// Option menu with submenu support from topology.
class nested_option_menu: 
public VSTGUI::COptionMenu,
public VSTGUI::IOptionMenuListener
{
  std::string _selected_item;
  VSTGUI::CMenuItemList _flat_list;
public:
  nested_option_menu();
  ~nested_option_menu();
  CLASS_METHODS(nested_option_menu, COptionMenu)

  void onOptionMenuPrePopup(VSTGUI::COptionMenu* menu) override;
  void onOptionMenuPostPopup(VSTGUI::COptionMenu* menu) override;
  bool onOptionMenuSetPopupResult(VSTGUI::COptionMenu* menu, VSTGUI::COptionMenu* selected, std::int32_t index) override;
};

// VSTGUI option menu factory.
class nested_option_menu_creator :
public VSTGUI::UIViewCreator::OptionMenuCreator
{
public:
  nested_option_menu_creator() {}
  VSTGUI::IdStringPtr getViewName() const override { return "inf_nested_option_menu"; }
  VSTGUI::IdStringPtr getBaseViewName() const override { return VSTGUI::UIViewCreator::kCOptionMenu; }
  VSTGUI::CView* create(VSTGUI::UIAttributes const& attrs, VSTGUI::IUIDescription const* desc) const override;
  bool apply(VSTGUI::CView* view, VSTGUI::UIAttributes const& attrs, VSTGUI::IUIDescription const* desc) const override;
};

} // namespace inf::base::ui
#endif // INF_BASE_UI_CONTROLS_NESTED_OPTION_MENU_HPP