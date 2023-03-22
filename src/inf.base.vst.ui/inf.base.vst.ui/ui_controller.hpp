#ifndef INF_BASE_VST_UI_UI_CONTROLLER_HPP
#define INF_BASE_VST_UI_UI_CONTROLLER_HPP

#include <inf.base.vst/sdk/controller.hpp>
#include <inf.base.vst.ui/editor.hpp>
#include <inf.base/topology/topology_info.hpp>

#include <pluginterfaces/vst/vsttypes.h>
#include <vstgui/plugin-bindings/vst3editor.h>
#include <public.sdk/source/vst/vsteditcontroller.h>

#include <vector>

namespace inf::base::vst::ui {

// Vst edit controller dynamically generated from topology_info.
class vst_ui_controller: 
public inf::base::vst::vst_controller, 
public VSTGUI::VST3EditorDelegate
{
  using IPlugView = Steinberg::IPlugView;  

  // Context menu.
  // Apply = source begin, target begin, param count.
  void add_patch_items(VSTGUI::COptionMenu* menu);
  void add_clear_items(VSTGUI::COptionMenu* menu);
  void add_preset_select_items(VSTGUI::COptionMenu* menu);
  void add_copy_swap_menu_items(VSTGUI::COptionMenu* menu, 
    std::string const& action, std::string const& target_prefix,
    std::function<void(std::int32_t, std::int32_t, std::int32_t)> apply);

public:
  // Update the editor ui if dependent params change.
  void view_removed(IPlugView* editor);
  void view_attached(IPlugView* editor);
  tresult endEdit(ParamID tag) override;

  explicit vst_ui_controller(std::unique_ptr<inf::base::topology_info>&& topology);
  IPlugView* PLUGIN_API createView(char const* name) override;
  VSTGUI::COptionMenu* createContextMenu(VSTGUI::CPoint const& pos, VSTGUI::VST3Editor* editor) override;
};

} // namespace inf::base::vst::ui
#endif // INF_BASE_VST_UI_UI_CONTROLLER_HPP