#ifndef INF_BASE_UI_CONTROLS_VIEW_SWITCH_CONTAINER_FIX_HPP
#define INF_BASE_UI_CONTROLS_VIEW_SWITCH_CONTAINER_FIX_HPP

#include <inf.base/topology/topology_info.hpp>
#include <vstgui/vstgui.h>
#include <vstgui/vstgui_uidescription.h>
#include <vstgui/uidescription/uiviewswitchcontainer.h>
#include <vstgui/uidescription/detail/uiviewcreatorattributes.h>
#include <vstgui/uidescription/viewcreator/uiviewswitchcontainercreator.h>

namespace inf::base::ui {

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
  topology_info const* const _topology;
protected:
  void valueChanged(VSTGUI::CControl* control) override;
public:
  view_switch_controller_fix(
    topology_info const* topology, VSTGUI::UIViewSwitchContainer* view_switch,
    VSTGUI::IUIDescription const* desc, VSTGUI::IController* controller):
  _topology(topology), UIDescriptionViewSwitchController(view_switch, desc, controller) {}
};

// VSTGUI container factory.
class view_switch_container_fix_creator :
public VSTGUI::UIViewCreator::UIViewSwitchContainerCreator
{
  topology_info const* const _topology;
public:
  view_switch_container_fix_creator(topology_info const* topology): _topology(topology) {}
  VSTGUI::IdStringPtr getViewName() const override { return "inf_view_switch_container_fix"; }
  VSTGUI::CView* create(VSTGUI::UIAttributes const& attrs, VSTGUI::IUIDescription const* desc) const override;
  VSTGUI::IdStringPtr getBaseViewName() const override { return VSTGUI::UIViewCreator::kUIViewSwitchContainer; }
  bool apply(VSTGUI::CView* view, VSTGUI::UIAttributes const& attrs, VSTGUI::IUIDescription const* desc) const override;
};

} // namespace inf::base::ui
#endif // INF_BASE_UI_CONTROLS_VIEW_SWITCH_CONTAINER_FIX_HPP