#ifndef SVN_VST_BASE_UI_OPTION_MENU_FIX_HPP
#define SVN_VST_BASE_UI_OPTION_MENU_FIX_HPP

#include <inf.base/topology/topology_info.hpp>

#include <vstgui/vstgui.h>
#include <vstgui/vstgui_uidescription.h>
#include <vstgui/lib/controls/coptionmenu.h>
#include <vstgui/uidescription/viewcreator/optionmenucreator.h>
#include <vstgui/uidescription/detail/uiviewcreatorattributes.h>

namespace inf::vst::base {

// Option menu with items-per-column support.
class option_menu_fix: 
public VSTGUI::COptionMenu
{
  static inline std::int32_t constexpr max_items_per_column = 32;
public:
  CLASS_METHODS(option_menu_fix, COptionMenu)
};

// VSTGUI option menu factory.
class option_menu_fix_creator :
public VSTGUI::UIViewCreator::OptionMenuCreator
{
public:
  VSTGUI::IdStringPtr getViewName() const override { return "inf_option_menu_fix"; }
  VSTGUI::IdStringPtr getBaseViewName() const override { return VSTGUI::UIViewCreator::kCOptionMenu; }
  VSTGUI::CView* create(VSTGUI::UIAttributes const& attrs, VSTGUI::IUIDescription const* desc) const override;
  bool apply(VSTGUI::CView* view, VSTGUI::UIAttributes const& attrs, VSTGUI::IUIDescription const* desc) const override;
};

} // namespace inf::vst::base
#endif // SVN_VST_BASE_UI_OPTION_MENU_FIX_HPP