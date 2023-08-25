// Some VST header missing an include.
#include <cassert>

#include <base/source/fstring.h>
#include <base/source/fstreamer.h>
#include <pluginterfaces/base/ibstream.h>
#include <pluginterfaces/vst/ivstcontextmenu.h>
#include <public.sdk/source/vst/vstpresetfile.h>

// Why? Is it missing from vst's cmake?
// In any case, can't link without it.
#include <public.sdk/source/common/memorystream.h>
#include <public.sdk/source/common/memorystream.cpp>

#include <inf.base/shared/support.hpp>
#include <inf.base.format.vst3/vst_parameter.hpp>
#include <inf.base.format.vst3/vst_editor.hpp>
#include <inf.base.format.vst3/vst_controller.hpp>
#include <inf.base.format.vst3/vst_support.hpp>
#include <inf.base.format.vst3/vst_io_stream.hpp>

#include <vector>
#include <cstring>
#include <cstdint>
#include <filesystem>

using namespace inf::base;
using namespace Steinberg;
using namespace Steinberg::Vst;                  

namespace inf::base::format::vst3 {

class vst_host_context_menu:
public host_context_menu
{
  Steinberg::IPtr<Steinberg::Vst::IContextMenu> _menu;
public:
  void item_clicked(std::int32_t index) override;
  host_context_menu_item get_item(std::int32_t index) const override;
  std::int32_t item_count() const override { return _menu->getItemCount(); }
  vst_host_context_menu(Steinberg::IPtr<Steinberg::Vst::IContextMenu> menu) : _menu(menu) {}
};

void 
vst_host_context_menu::item_clicked(std::int32_t index)
{
  IContextMenu::Item item;
  IContextMenuTarget* target;
  _menu->getItem(index, item, &target);
  if(target == nullptr) return;
  target->executeMenuItem(item.tag);
}

host_context_menu_item
vst_host_context_menu::get_item(std::int32_t index) const
{
  IContextMenu::Item item;
  IContextMenuTarget* target;
  host_context_menu_item result = {};
  _menu->getItem(index, item, &target);
  result.name = from_vst_string(item.name);
  if ((item.flags & IContextMenuItem::kIsChecked) != 0) result.flags |= host_context_menu_item::checked;
  if ((item.flags & IContextMenuItem::kIsDisabled) == 0) result.flags |= host_context_menu_item::enabled;
  if ((item.flags & IContextMenuItem::kIsSeparator) != 0) result.flags |= host_context_menu_item::separator;
  if ((item.flags & IContextMenuItem::kIsGroupEnd) != 0) result.flags |= host_context_menu_item::group_end;
  if ((item.flags & IContextMenuItem::kIsGroupStart) != 0) result.flags |= host_context_menu_item::group_start;
  return result;
}

vst_controller::
vst_controller(std::unique_ptr<inf::base::topology_info>&& topology, FUID const& processor_id) :
plugin_controller(std::move(topology)), _processor_id(processor_id) {}

void* 
vst_controller::current_editor_window() const
{ 
  if(_current_editor == nullptr) return nullptr;
  return _current_editor->current_window(); 
}

void
vst_controller::reload_editor(std::int32_t width)
{
  if (_current_editor == nullptr) return;
  _current_editor->set_width(width);
}

std::u16string
vst_controller::host_name() const
{
  String128 name;
  IPtr<IHostApplication> app;
  if(hostContext->queryInterface(IHostApplication::iid, reinterpret_cast<void**>(&app)) != kResultOk) return {};
  if(app->getName(name) != kResultOk) return {};
  return std::u16string(name);
}

tresult PLUGIN_API 
vst_controller::getMidiControllerAssignment(int32 bus_index, int16 channel, CtrlNumber midi_ctrl_nr, ParamID& id)
{
  // VST3PluginTestHost crashes if we map any midi controller.
  if(std::u16string(u"VST3PluginTestHost Standalone") == host_name()) return kResultFalse;
  std::int32_t target_tag;
  if(bus_index != 0) return kResultFalse;
  if(!map_midi_control(midi_ctrl_nr, target_tag)) return kResultFalse;
  id = static_cast<ParamID>(target_tag);
  return kResultTrue;
}

void 
vst_controller::restart() 
{ 
  if (componentHandler != nullptr) 
    componentHandler->restartComponent(Steinberg::Vst::kParamValuesChanged); 
  sendTextMessage(hard_reset_request_msg_id);
}

IPlugView* PLUGIN_API
vst_controller::createView(char const* name)
{
  if (ConstString(name) != ViewType::kEditor) return nullptr;
  _current_editor = create_editor();
  setKnobMode(KnobModes::kLinearMode);
  return _current_editor;
}

void
vst_controller::do_edit(std::int32_t tag, double normalized)
{
  beginEdit(tag);
  setParamNormalized(tag, normalized);
  performEdit(tag, getParamNormalized(tag));
  endEdit(tag);
}

void 
vst_controller::editor_param_changed(std::int32_t index, param_value ui_value)
{
  param_value base_value = topology()->ui_to_base_value(index, ui_value);
  std::int32_t tag = topology()->param_index_to_id[index];
  do_edit(tag, base_to_format_normalized(topology(), true, index, base_value));
}

void
vst_controller::swap_param(std::int32_t source_tag, std::int32_t target_tag)
{
  ParamValue target_value = getParamNormalized(target_tag);
  do_edit(target_tag, getParamNormalized(source_tag));
  do_edit(source_tag, target_value);
}

tresult PLUGIN_API
vst_controller::setParamNormalized(ParamID tag, ParamValue value)
{
  tresult result = EditController::setParamNormalized(tag, value);
  if(result != kResultOk) return result;
  update_state(tag);
  std::int32_t index = topology()->param_id_to_index.at(tag);
  controller_param_changed(tag, format_normalized_to_base(topology(), true, index, value));
  return kResultOk;
}

std::string 
vst_controller::themes_folder(std::string const& plugin_file) const
{
  auto path = std::filesystem::path(plugin_file);
  return (path.parent_path().parent_path() / "Resources" / "Themes").string();
}

std::string 
vst_controller::factory_presets_folder(std::string const& plugin_file) const
{
  auto path = std::filesystem::path(plugin_file);
  return (path.parent_path().parent_path() / "Resources" / "Presets").string();
}

tresult 
vst_controller::setState(IBStream* state)
{
  if(state == nullptr) return kResultFalse;
  IBStreamer streamer(state, kLittleEndian);
  vst_io_stream stream(&streamer);
  stream.load_controller(*topology(), patch_meta_data());
  reloaded();
  return kResultOk;
}

tresult 
vst_controller::getState(IBStream* state)
{
  if (state == nullptr) return kResultFalse;
  IBStreamer streamer(state, kLittleEndian);
  vst_io_stream stream(&streamer);
  stream.save_controller(*topology(), patch_meta_data());
  return kResultOk;
}

tresult
vst_controller::set_component_state(IBStream* state)
{
  if (state == nullptr) return kResultFalse;

  // Load state into temporary buffer.
  IBStreamer streamer(state, kLittleEndian);
  vst_io_stream stream(&streamer);
  std::vector<param_value> values(_topology->input_param_count, param_value());
  if (!stream.load_processor(*_topology, values.data())) return kResultFalse;
  load_component_state(values.data());
  return kResultOk;
}

// Update private copy of the state in inf::base format, for easy access by graphs.
void
vst_controller::update_state(ParamID tag)
{
  double normalized = getParamNormalized(tag);
  std::int32_t index = _topology->param_id_to_index[tag];
  assert(index >= 0 && index < static_cast<std::int32_t>(_topology->params.size()));
  _state[index] = format_normalized_to_base(_topology.get(), true, index, normalized);
}

void 
vst_controller::load_component_state(param_value* state)
{
  // SetParamNormalized() each value.
  for (std::int32_t p = 0; p < _topology->input_param_count; p++)
  {
    ParamID tag = _topology->param_index_to_id[p];
    ParamValue value = base_to_format_normalized(_topology.get(), true, p, state[p]);
    do_edit(tag, value);
    update_state(tag);
  }
  restart();
}

tresult PLUGIN_API
vst_controller::initialize(FUnknown* context)
{
  tresult result = EditController::initialize(context);
  if (result != kResultTrue) return result;

  // Add all runtime parameters.
  for (std::int32_t p = 0; p < static_cast<std::int32_t>(_topology->params.size()); p++)
  {
    ParamID tag = _topology->param_index_to_id[p];
    part_info const* part = &_topology->parts[_topology->params[p].part_index];
    parameters.addParameter(new vst_parameter(tag, part, &_topology->params[p]));
  }
  return kResultTrue;
}

std::unique_ptr<host_context_menu>
vst_controller::host_menu_for_param_index(std::int32_t param_index) const
{
  Steinberg::FUnknownPtr<Steinberg::Vst::IComponentHandler3> handler(componentHandler);
  if (handler == nullptr) return {};
  if (!_current_editor) return {};

  ParamID tag = topology()->param_index_to_id[param_index];
  Steinberg::IPtr<Steinberg::Vst::IContextMenu> menu(handler->createContextMenu(_current_editor, &tag));
  return std::make_unique<vst_host_context_menu>(menu);
}

} // namespace inf::base::format::vst3