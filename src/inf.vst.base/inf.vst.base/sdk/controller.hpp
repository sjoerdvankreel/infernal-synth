#ifndef INF_VST_BASE_SDK_CONTROLLER_HPP
#define INF_VST_BASE_SDK_CONTROLLER_HPP

#include <inf.vst.base/ui/vst_editor.hpp>
#include <inf.base/topology/topology_info.hpp>

#include <pluginterfaces/vst/vsttypes.h>
#include <vstgui/plugin-bindings/vst3editor.h>
#include <public.sdk/source/vst/vsteditcontroller.h>

#include <vector>

namespace inf::vst::base {

// For factory presets (just every .vstpreset in the resources folder).
struct preset_item
{
  std::string name;
  std::string path;
};

// Vst edit controller dynamically generated from topology_info.
class vst_controller: 
public Steinberg::Vst::EditControllerEx1, 
public VSTGUI::VST3EditorDelegate
{
  using tresult = Steinberg::tresult;
  using IBStream = Steinberg::IBStream;
  using FUnknown = Steinberg::FUnknown;
  using IPlugView = Steinberg::IPlugView;
  using ParamID = Steinberg::Vst::ParamID;
  
  vst_editor* _editor;
  bool _preset_items_initialized;
  std::vector<preset_item> _preset_items;
  // Separate copy of the parameter state used for graphs.
  std::vector<inf::base::param_value> _state;
  std::unique_ptr<inf::base::topology_info> _topology;

  // Context menu.
  // Apply = source begin, target begin, param count.
  void add_init_items(VSTGUI::COptionMenu* menu);
  void add_preset_select_items(VSTGUI::COptionMenu* menu);
  void add_copy_swap_menu_items(VSTGUI::COptionMenu* menu, 
    std::string const& action, std::string const& target_prefix,
    std::function<void(std::int32_t, std::int32_t, std::int32_t)> apply);

private:
  void update_state(ParamID tag);
  void load_preset(std::size_t index);
  void clear_module(std::int32_t type, std::int32_t index);
  tresult set_component_state(IBStream* state, bool perform_edit);
  void load_component_state(inf::base::param_value* state, bool perform_edit);

public:
  // Update the editor ui if dependent params change.
  void sync_ui_parameters();
  void view_removed(vst_editor* editor);
  void view_attached(vst_editor* editor);

  explicit vst_controller(std::unique_ptr<inf::base::topology_info>&& topology) :
  _editor(nullptr), _preset_items_initialized(false), _preset_items(),
  _state(topology->params.size()), _topology(std::move(topology)) {}

  tresult endEdit(ParamID tag) override;
  tresult PLUGIN_API initialize(FUnknown* context) override;
  IPlugView* PLUGIN_API createView(char const* name) override;
  VSTGUI::COptionMenu* createContextMenu(VSTGUI::CPoint const& pos, VSTGUI::VST3Editor* editor) override;

  inf::base::param_value const* state() const { return _state.data(); }
  tresult PLUGIN_API setComponentState(IBStream* state) override { return set_component_state(state, false); }
};

} // namespace inf::vst::base
#endif // INF_VST_BASE_SDK_CONTROLLER_HPP
