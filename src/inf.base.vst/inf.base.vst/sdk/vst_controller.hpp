#ifndef INF_BASE_VST_SDK_VST_CONTROLLER_HPP
#define INF_BASE_VST_SDK_VST_CONTROLLER_HPP

#include <inf.base/plugin/plugin_controller.hpp>
#include <inf.base/topology/topology_info.hpp>
#include <inf.base.vst/shared/support.hpp>

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

  double get_plugin_param(std::int32_t tag) override { return getParamNormalized(tag); }
  void restart() override { componentHandler->restartComponent(Steinberg::Vst::kParamValuesChanged); }
  tresult PLUGIN_API setComponentState(IBStream* state) override { return set_component_state(state, false); }
  double base_to_plugin_param(std::int32_t index, param_value val) const override { return base_to_vst_normalized(topology(), index, val); }
  param_value plugin_to_base_param(std::int32_t index, double val) const override { return vst_normalized_to_base(topology(), index, val); }
};

} // namespace inf::base::vst
#endif // INF_BASE_VST_SDK_VST_CONTROLLER_HPP
