#ifndef INF_BASE_FORMAT_VST3_VST_CONTROLLER_HPP
#define INF_BASE_FORMAT_VST3_VST_CONTROLLER_HPP

#include <inf.base/plugin/plugin_controller.hpp>
#include <inf.base/topology/topology_info.hpp>
#include <inf.base.format.vst3/vst_support.hpp>

#include <pluginterfaces/vst/vsttypes.h>
#include <public.sdk/source/vst/vsteditcontroller.h>

#include <vector>

namespace inf::base::format::vst3 {

class vst_editor;

// Vst edit controller.
class vst_controller: 
public inf::base::plugin_controller,
public Steinberg::Vst::EditController,
public Steinberg::Vst::IMidiMapping
{
protected:
  using FUID = Steinberg::FUID;
  using int16 = Steinberg::int16;
  using int32 = Steinberg::int32;
  using tresult = Steinberg::tresult;
  using IBStream = Steinberg::IBStream;
  using FUnknown = Steinberg::FUnknown;
  using IPlugView = Steinberg::IPlugView;
  using ParamID = Steinberg::Vst::ParamID;
  using ParamValue = Steinberg::Vst::ParamValue;
  using CtrlNumber = Steinberg::Vst::CtrlNumber;

  FUID const _processor_id;
  vst_editor* _current_editor = nullptr;

  void do_edit(std::int32_t tag, double normalized);

protected:
  void update_state(ParamID tag);
  std::u16string host_name() const;
  virtual vst_editor* create_editor() = 0;
  tresult set_component_state(IBStream* state);

  virtual bool map_midi_control(std::int32_t number, std::int32_t& target_tag) const = 0;
  vst_controller(std::unique_ptr<inf::base::topology_info>&& topology, FUID const& processor_id);

public:
  void* current_editor_window() const override;
  void reload_editor(std::int32_t width) override;
  void editor_param_changed(std::int32_t index, param_value ui_value) override;

  void restart() override;
  void load_component_state(inf::base::param_value* state) override;
  void swap_param(std::int32_t source_tag, std::int32_t target_tag) override;
  void copy_param(std::int32_t source_tag, std::int32_t target_tag) override 
  { do_edit(target_tag, getParamNormalized(source_tag)); }

  std::string default_theme_name() const override { return "3D Default"; }
  std::string themes_folder(std::string const& plugin_file) const override;
  std::string factory_presets_folder(std::string const& plugin_file) const override;
  std::unique_ptr<host_context_menu> host_menu_for_param_index(std::int32_t param_index) const override;

  tresult PLUGIN_API setState(IBStream* state) override;
  tresult PLUGIN_API getState(IBStream* state) override;
  tresult PLUGIN_API initialize(FUnknown* context) override;
  IPlugView* PLUGIN_API createView(char const* name) override;
  tresult PLUGIN_API setParamNormalized(ParamID tag, ParamValue value) override;
  tresult PLUGIN_API setComponentState(IBStream* state) override { return set_component_state(state); }
  tresult PLUGIN_API getMidiControllerAssignment(int32 bus_index, int16 channel, CtrlNumber midi_ctrl_nr, ParamID& id) override;

  OBJ_METHODS(vst_controller, EditController)
  DEFINE_INTERFACES
    DEF_INTERFACE(IMidiMapping)
  END_DEFINE_INTERFACES(EditController)
  REFCOUNT_METHODS(EditController)
};

} // namespace inf::base::format::vst3
#endif // INF_BASE_FORMAT_VST3_VST_CONTROLLER_HPP
