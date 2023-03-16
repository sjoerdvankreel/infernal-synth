#ifndef INF_VST_SDK_CONTROLLER_HPP
#define INF_VST_SDK_CONTROLLER_HPP

#include <inf.base/topology/topology_info.hpp>
#include <pluginterfaces/vst/vsttypes.h>
#include <public.sdk/source/vst/vsteditcontroller.h>

#include <vector>

namespace inf::vst {

// For factory presets (just every .vstpreset in the resources folder).
struct preset_item
{
  std::string name;
  std::string path;
};

// Vst edit controller dynamically generated from topology_info.
class vst_controller: 
public Steinberg::Vst::EditControllerEx1
{
protected:
  using tresult = Steinberg::tresult;
  using IBStream = Steinberg::IBStream;
  using FUnknown = Steinberg::FUnknown;
  using IPlugView = Steinberg::IPlugView;
  using ParamID = Steinberg::Vst::ParamID;
  
  IPlugView* _editor;
  bool _preset_items_initialized;
  std::vector<preset_item> _preset_items;
  // Separate copy of the parameter state used for graphs.
  std::vector<inf::base::param_value> _state;
  std::unique_ptr<inf::base::topology_info> _topology;

private:
  void update_state(ParamID tag);
  void load_preset(std::size_t index);
  void clear_module(std::int32_t type, std::int32_t index);
  tresult set_component_state(IBStream* state, bool perform_edit);
  void load_component_state(inf::base::param_value* state, bool perform_edit);

public:
  // Update the editor ui if dependent params change.
  void sync_ui_parameters();
  void view_removed(IPlugView* editor);
  void view_attached(IPlugView* editor);
  explicit vst_controller(std::unique_ptr<inf::base::topology_info>&& topology);

  tresult endEdit(ParamID tag) override;
  tresult PLUGIN_API initialize(FUnknown* context) override;
  inf::base::param_value const* state() const { return _state.data(); }
  tresult PLUGIN_API setComponentState(IBStream* state) override { return set_component_state(state, false); }
};

} // namespace inf::vst
#endif // INF_VST_SDK_CONTROLLER_HPP
