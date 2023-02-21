#ifndef SVN_VST_BASE_UI_VIEW_CONTAINER_FIX_HPP
#define SVN_VST_BASE_UI_VIEW_CONTAINER_FIX_HPP

#include <vstgui/vstgui.h>
#include <vstgui/vstgui_uidescription.h>
#include <vstgui/uidescription/detail/uiviewcreatorattributes.h>
#include <vstgui/uidescription/viewcreator/viewcontainercreator.h>

namespace svn::vst::base {

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
  VSTGUI::IdStringPtr getViewName() const override { return "svn_view_container_fix"; }
  VSTGUI::IdStringPtr getBaseViewName() const override { return VSTGUI::UIViewCreator::kCViewContainer; }
  VSTGUI::CView* create(VSTGUI::UIAttributes const& attrs, VSTGUI::IUIDescription const* desc) const override;
  bool apply(VSTGUI::CView* view, VSTGUI::UIAttributes const& attrs, VSTGUI::IUIDescription const* desc) const override;
};

} // namespace svn::vst::base
#endif // SVN_VST_BASE_UI_VIEW_CONTAINER_FIX_HPP