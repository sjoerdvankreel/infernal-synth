#include <svn.vst.base/ui/option_menu_fix.hpp>
#include <vstgui/uidescription/uiviewcreator.h>

using namespace svn::base;
using namespace VSTGUI;
using namespace VSTGUI::UIViewCreator;

namespace svn::vst::base {
  
CView* 
option_menu_fix_creator::create(
  UIAttributes const& attrs, IUIDescription const* desc) const
{ return new option_menu_fix(); }
 
bool 
option_menu_fix_creator::apply(
  CView* view, UIAttributes const& attrs, IUIDescription const* desc) const
{ 
  std::int32_t items_per_column;
  if(!OptionMenuCreator::apply(view, attrs, desc)) return false; 
  if(!attrs.getIntegerAttribute("items-per-column", items_per_column)) return false;
  dynamic_cast<option_menu_fix&>(*view).setNbItemsPerColumn(items_per_column);
  return true;
}

} // namespace svn::vst::base