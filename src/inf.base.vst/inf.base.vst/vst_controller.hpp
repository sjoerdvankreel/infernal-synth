#ifndef INF_BASE_VST_VST_CONTROLLER_HPP
#define INF_BASE_VST_VST_CONTROLLER_HPP

#include <inf.base/plugin/plugin_controller.hpp>
#include <inf.base/topology/topology_info.hpp>
#include <inf.base.vst/vst_support.hpp>

#include <pluginterfaces/vst/vsttypes.h>
#include <public.sdk/source/vst/vsteditcontroller.h>

#include <vector>

namespace inf::base::vst {

class vst_editor;

// Vst edit controller.
class vst_controller: 
public Steinberg::Vst::EditController,
public inf::base::plugin_controller
{
protected:
  using FUID = Steinberg::FUID;
  using tresult = Steinberg::tresult;
  using IBStream = Steinberg::IBStream;
  using FUnknown = Steinberg::FUnknown;
  using IPlugView = Steinberg::IPlugView;
  using ParamID = Steinberg::Vst::ParamID;
  using ParamValue = Steinberg::Vst::ParamValue;

  FUID const _processor_id;
  std::int32_t _editor_width = 0;
  vst_editor* _current_editor = nullptr;

  void do_edit(std::int32_t tag, double normalized);

protected:
  void update_state(ParamID tag);
  virtual vst_editor* create_editor() = 0;
  tresult set_component_state(IBStream* state, bool perform_edit);
  vst_controller(std::unique_ptr<inf::base::topology_info>&& topology, FUID const& processor_id);

public:
  std::int32_t editor_current_width() const override { return _editor_width; }
  void editor_current_width(std::int32_t editor_width) { _editor_width = editor_width; }

  void* current_editor_window() const override;
  void save_preset(std::string const& path) override;
  tresult PLUGIN_API initialize(FUnknown* context) override;
  IPlugView* PLUGIN_API createView(char const* name) override;
  bool load_preset(std::string const& path, bool factory) override;
  void swap_param(std::int32_t source_tag, std::int32_t target_tag) override;
  void editor_param_changed(std::int32_t index, param_value ui_value) override;
  tresult PLUGIN_API setParamNormalized(ParamID tag, ParamValue value) override;
  void load_component_state(inf::base::param_value* state, bool perform_edit) override;

  std::string preset_file_extension() override { return "vstpreset"; }
  double get_plugin_param(std::int32_t tag) override { return getParamNormalized(tag); }
  tresult PLUGIN_API setComponentState(IBStream* state) override { return set_component_state(state, false); }
  void copy_param(std::int32_t source_tag, std::int32_t target_tag) override { do_edit(target_tag, getParamNormalized(source_tag)); }
  void restart() override { if (componentHandler != nullptr) componentHandler->restartComponent(Steinberg::Vst::kParamValuesChanged); }
  double base_to_plugin_param(std::int32_t index, param_value val) const override { return base_to_vst_normalized(topology(), index, val); }
  param_value plugin_to_base_param(std::int32_t index, double val) const override { return vst_normalized_to_base(topology(), index, val); }
};

} // namespace inf::base::vst
#endif // INF_BASE_VST_VST_CONTROLLER_HPP
