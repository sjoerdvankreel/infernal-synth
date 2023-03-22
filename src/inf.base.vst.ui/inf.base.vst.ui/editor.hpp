#ifndef INF_BASE_VST_UI_EDITOR_HPP
#define INF_BASE_VST_UI_EDITOR_HPP

#include <inf.base/topology/topology_info.hpp>
#include <inf.base.ui/controls/graph_plot.hpp>
#include <vstgui/plugin-bindings/vst3editor.h>
#include <vector>

namespace inf::base::vst::ui {

// Vst3 editor with basic support for declarative parameter 
// visibility. See param_ui_descriptor.relevant_if_param.
// Json files (.uidesc) are generated by ui generator project.
// Note: this class breaks the vst3 gui editor. Remove 
// onViewAdded/onViewRemoved/update_dependent_visibility if you need
// to play around with the editor. Conditional visibility 
// obviously won't work then.
class vst_editor: 
public VSTGUI::VST3Editor
{
  using CView = VSTGUI::CView;
  using CFrame = VSTGUI::CFrame;
  using CControl = VSTGUI::CControl;
  using ParamID = Steinberg::Vst::ParamID;
  using PlatformType = VSTGUI::PlatformType;
  using UTF8StringPtr = VSTGUI::UTF8StringPtr;
  using EditController = Steinberg::Vst::EditController;
  
  std::vector<base::ui::graph_plot*> _graphs;
  std::vector<std::vector<CControl*>> _controls;
  inf::base::topology_info const* const _topology;

public:
  // Let controller know we live or die.
  void attachedToParent() override;
  void removedFromParent() override;

  // Keeping track of graphs and controls by tag for fast access.
  void onViewAdded(CFrame* view_frame, CView* view) override;
  void onViewRemoved(CFrame* view_frame, CView* view) override;

  // Update visibility of dependent views and rerender graphs.
  void update_dependent_visibility(ParamID tag);
  bool PLUGIN_API open(void* parent, const PlatformType& type) override;

  inf::base::topology_info const* topology() const { return _topology; }
  vst_editor(EditController* controller, UTF8StringPtr template_name,
    UTF8StringPtr xml_file, inf::base::topology_info const* topology);

  // Expose IParameterFinder.
  Steinberg::tresult PLUGIN_API find_parameter(VSTGUI::CPoint const& pos, Steinberg::Vst::ParamID& id)
  { return findParameter(pos.x, pos.y, id); }
};

} // namespace inf::base::vst::ui
#endif // INF_BASE_VST_UI_EDITOR_HPP
