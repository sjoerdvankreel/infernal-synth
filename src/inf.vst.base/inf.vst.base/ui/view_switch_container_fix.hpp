#ifndef INF_VST_BASE_UI_VIEW_SWITCH_CONTAINER_FIX_HPP
#define INF_VST_BASE_UI_VIEW_SWITCH_CONTAINER_FIX_HPP

#include <vstgui/vstgui.h>
#include <vstgui/vstgui_uidescription.h>
#include <vstgui/uidescription/uiviewswitchcontainer.h>
#include <vstgui/uidescription/detail/uiviewcreatorattributes.h>
#include <vstgui/uidescription/viewcreator/uiviewswitchcontainercreator.h>

namespace inf::vst::base {

// View switch container triggering repaint for conditional visibility.
class view_switch_container_fix : 
public VSTGUI::UIViewSwitchContainer
{
public:
  CLASS_METHODS(view_switch_container_fix, VSTGUI::UIViewSwitchContainer)
  view_switch_container_fix(): UIViewSwitchContainer(VSTGUI::CRect(0, 0, 0, 0)) {}
};

// View switch container triggering repaint for conditional visibility.
class view_switch_controller_fix :
public VSTGUI::UIDescriptionViewSwitchController
{
protected:
  void valueChanged(VSTGUI::CControl* control) override;
public:
  view_switch_controller_fix(VSTGUI::UIViewSwitchContainer* view_switch, 
    VSTGUI::IUIDescription const* desc, VSTGUI::IController* controller):
  UIDescriptionViewSwitchController(view_switch, desc, controller) {}
};

// VSTGUI container factory.
class view_switch_container_fix_creator :
public VSTGUI::UIViewCreator::UIViewSwitchContainerCreator
{
public:
  VSTGUI::IdStringPtr getViewName() const override { return "inf_view_switch_container_fix"; }
  VSTGUI::CView* create(VSTGUI::UIAttributes const& attrs, VSTGUI::IUIDescription const* desc) const override;
  VSTGUI::IdStringPtr getBaseViewName() const override { return VSTGUI::UIViewCreator::kUIViewSwitchContainer; }
  bool apply(VSTGUI::CView* view, VSTGUI::UIAttributes const& attrs, VSTGUI::IUIDescription const* desc) const override;
};

} // namespace inf::vst::base
#endif // INF_VST_BASE_UI_VIEW_SWITCH_CONTAINER_FIX_HPP