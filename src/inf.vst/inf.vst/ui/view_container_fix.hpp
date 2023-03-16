#ifndef INF_VST_UI_VIEW_CONTAINER_FIX_HPP
#define INF_VST_UI_VIEW_CONTAINER_FIX_HPP

#include <vstgui/vstgui.h>
#include <vstgui/vstgui_uidescription.h>
#include <vstgui/uidescription/detail/uiviewcreatorattributes.h>
#include <vstgui/uidescription/viewcreator/viewcontainercreator.h>

namespace inf::vst {

// View container respecting bitmap offset.
class view_container_fix : 
public VSTGUI::CViewContainer
{
public:
  CLASS_METHODS(view_container_fix, CViewContainer)
  view_container_fix(): CViewContainer(VSTGUI::CRect(0, 0, 0, 0)) {}
};

// VSTGUI container factory.
class view_container_fix_creator :
public VSTGUI::UIViewCreator::ViewContainerCreator
{
public:
  VSTGUI::IdStringPtr getViewName() const override { return "inf_view_container_fix"; }
  VSTGUI::IdStringPtr getBaseViewName() const override { return VSTGUI::UIViewCreator::kCViewContainer; }
  VSTGUI::CView* create(VSTGUI::UIAttributes const& attrs, VSTGUI::IUIDescription const* desc) const override;
  bool apply(VSTGUI::CView* view, VSTGUI::UIAttributes const& attrs, VSTGUI::IUIDescription const* desc) const override;
};

} // namespace inf::vst
#endif // INF_VST_UI_VIEW_CONTAINER_FIX_HPP