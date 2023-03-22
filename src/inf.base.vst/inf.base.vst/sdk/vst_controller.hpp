#ifndef INF_BASE_VST_SDK_VST_CONTROLLER_HPP
#define INF_BASE_VST_SDK_VST_CONTROLLER_HPP

#include <inf.base/plugin/plugin_controller.hpp>
#include <inf.base/topology/topology_info.hpp>
#include <pluginterfaces/vst/vsttypes.h>
#include <public.sdk/source/vst/vsteditcontroller.h>

#include <vector>

namespace inf::base::vst {

// Vst edit controller.
class vst_controller: 
public Steinberg::Vst::EditControllerEx1,
public inf::base::plugin_controller
{
protected:
  using tresult = Steinberg::tresult;
  using IBStream = Steinberg::IBStream;
  using FUnknown = Steinberg::FUnknown;
  using ParamID = Steinberg::Vst::ParamID;
  
  // Separate copy of the parameter state used for graphs.
  std::vector<inf::base::param_value> _state;
  std::vector<inf::base::preset_item> _preset_items;
  std::unique_ptr<inf::base::topology_info> _topology;

protected:
  void update_state(ParamID tag);
  tresult set_component_state(IBStream* state, bool perform_edit);

public:
  explicit vst_controller(std::unique_ptr<inf::base::topology_info>&& topology);

  void sync_ui_parameters();
  void load_preset(std::size_t index) override;
  tresult PLUGIN_API initialize(FUnknown* context) override;
  void copy_param(std::int32_t source_tag, std::int32_t target_tag) override;
  void swap_param(std::int32_t source_tag, std::int32_t target_tag) override;
  void load_component_state(inf::base::param_value* state, bool perform_edit) override;

  inf::base::param_value const* state() const { return _state.data(); }
  inf::base::topology_info const* topology() const { return _topology.get(); }
  std::vector<inf::base::preset_item>& preset_items() { return _preset_items; }
  void restart() override { componentHandler->restartComponent(Steinberg::Vst::kParamValuesChanged); }
  tresult PLUGIN_API setComponentState(IBStream* state) override { return set_component_state(state, false); }
};

} // namespace inf::base::vst
#endif // INF_BASE_VST_SDK_VST_CONTROLLER_HPP
