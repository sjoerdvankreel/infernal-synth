#ifndef SVN_VST_BASE_UI_NESTED_OPTION_MENU_HPP
#define SVN_VST_BASE_UI_NESTED_OPTION_MENU_HPP

#include <svn.base/topology/topology_info.hpp>

#include <vstgui/vstgui.h>
#include <vstgui/vstgui_uidescription.h>
#include <vstgui/lib/controls/coptionmenu.h>
#include <vstgui/uidescription/viewcreator/optionmenucreator.h>
#include <vstgui/uidescription/detail/uiviewcreatorattributes.h>

namespace svn::vst::base {

// Option menu with submenu support from topology.
class nested_option_menu: 
public VSTGUI::COptionMenu,
public VSTGUI::IOptionMenuListener
{
  std::string _selected_item;
  VSTGUI::CMenuItemList _flat_list;
  svn::base::topology_info const* const _topology;
public:
  ~nested_option_menu();
  nested_option_menu(svn::base::topology_info const* topology);
  CLASS_METHODS(nested_option_menu, COptionMenu)

  void onOptionMenuPrePopup(VSTGUI::COptionMenu* menu) override;
  void onOptionMenuPostPopup(VSTGUI::COptionMenu* menu) override;
  bool onOptionMenuSetPopupResult(VSTGUI::COptionMenu* menu, VSTGUI::COptionMenu* selected, std::int32_t index) override;
};

// VSTGUI option menu factory.
class nested_option_menu_creator :
public VSTGUI::UIViewCreator::OptionMenuCreator
{
  svn::base::topology_info const* const _topology;
public:
  nested_option_menu_creator(svn::base::topology_info const* topology): _topology(topology) {}
  VSTGUI::IdStringPtr getViewName() const override { return "svn_nested_option_menu"; }
  VSTGUI::IdStringPtr getBaseViewName() const override { return VSTGUI::UIViewCreator::kCOptionMenu; }
  VSTGUI::CView* create(VSTGUI::UIAttributes const& attrs, VSTGUI::IUIDescription const* desc) const override;
  bool apply(VSTGUI::CView* view, VSTGUI::UIAttributes const& attrs, VSTGUI::IUIDescription const* desc) const override;
};

} // namespace svn::vst::base
#endif // SVN_VST_BASE_UI_NESTED_OPTION_MENU_HPP