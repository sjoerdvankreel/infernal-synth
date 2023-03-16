#include <inf.vst.base/ui/view_container_fix.hpp>
#include <vstgui/uidescription/uiviewcreator.h>

using namespace VSTGUI;
using namespace VSTGUI::UIViewCreator;

namespace inf::vst {

CView* 
view_container_fix_creator::create(
  UIAttributes const& attrs, IUIDescription const* desc) const
{ return new view_container_fix(); }

bool   
view_container_fix_creator::apply(
  CView* view, UIAttributes const& attrs, IUIDescription const* desc) const
{
  CPoint offset;
  if(!ViewContainerCreator::apply(view, attrs, desc)) return false;
  if (attrs.getPointAttribute(kAttrBackgroundOffset, offset))
    dynamic_cast<view_container_fix&>(*view).setBackgroundOffset(offset);
  return true;
}

} // namespace inf::vst