#ifndef INF_BASE_VST_UI_VST_UI_EDITOR_HPP
#define INF_BASE_VST_UI_VST_UI_EDITOR_HPP

#include <inf.base.ui/shared/plugin_ui_editor.hpp>
#include <inf.base.vst.ui/vst_ui_controller.hpp>

#include <vstgui/plugin-bindings/vst3editor.h>
#include <vector>

namespace inf::base::vst::ui {

class vst_ui_controller;

// Vst3 editor with basic support for declarative parameter 
// visibility. See param_ui_descriptor.relevant_if_param.
class vst_ui_editor: 
public inf::base::ui::plugin_ui_editor,
public Steinberg::Vst::VSTGUIEditor,
public VSTGUI::IViewAddedRemovedObserver
{
  using ParamID = Steinberg::Vst::ParamID;
  using EditController = Steinberg::Vst::EditController;

public:
  vst_ui_editor(vst_ui_controller* controller, inf::base::topology_info const* topology);

  void PLUGIN_API close() override;
  bool PLUGIN_API open(void* parent, const PlatformType& type) override;

  void attachedToParent() override { dynamic_cast<vst_ui_controller&>(*getController()).view_attached(this); }
  void removedFromParent() override { dynamic_cast<vst_ui_controller&>(*getController()).view_removed(this); }
  void onViewAdded(CFrame* view_frame, CView* view) override { plugin_ui_editor::view_added(view_frame, view); }
  void onViewRemoved(CFrame* view_frame, CView* view) override { plugin_ui_editor::view_removed(view_frame, view); }
  //Steinberg::tresult PLUGIN_API find_parameter(VSTGUI::CPoint const& pos, Steinberg::Vst::ParamID& id) { return findParameter(pos.x, pos.y, id); }
};

} // namespace inf::base::vst::ui
#endif // INF_BASE_VST_UI_VST_UI_EDITOR_HPP
