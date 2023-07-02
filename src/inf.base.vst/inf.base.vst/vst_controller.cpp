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
#include <inf.base.vst/vst_parameter.hpp>
#include <inf.base.vst/vst_editor.hpp>
#include <inf.base.vst/vst_controller.hpp>
#include <inf.base.vst/vst_support.hpp>
#include <inf.base.vst/vst_io_stream.hpp>

#include <vector>
#include <cstring>
#include <cstdint>
#include <fstream>
#include <filesystem>

using namespace inf::base;
using namespace Steinberg;
using namespace Steinberg::Vst;                  

namespace inf::base::vst {

class vst_host_context_menu:
public host_context_menu
{
  Steinberg::IPtr<Steinberg::Vst::IContextMenu> _menu;
public:
  std::int32_t item_count() const override { return _menu->getItemCount(); }
  vst_host_context_menu(Steinberg::IPtr<Steinberg::Vst::IContextMenu> menu) : _menu(menu) {}

  void item_clicked(std::int32_t index) override;
  void get_item(std::int32_t index, std::string& name, bool& enabled, bool& checked) const override;
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

void
vst_host_context_menu::get_item(std::int32_t index, std::string& name, bool& enabled, bool& checked) const
{
  IContextMenu::Item item;
  IContextMenuTarget* target;
  _menu->getItem(index, item, &target);
  name = from_vst_string(item.name);
  checked = (item.flags & IContextMenuItem::kIsChecked) != 0;
  enabled = (item.flags & IContextMenuItem::kIsDisabled) == 0;
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
  do_edit(tag, base_to_vst_normalized(topology(), index, base_value));
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
  controller_param_changed(tag, vst_normalized_to_base(topology(), index, static_cast<float>(value)));
  return kResultOk;
}

std::string 
vst_controller::default_theme_path(std::string const& plugin_file) const
{
  auto path = std::filesystem::path(plugin_file);
  return (path.parent_path().parent_path() / "Resources" / "Themes" / "Default Theme").string();
}

std::vector<inf::base::external_resource> 
vst_controller::themes(std::string const& plugin_file) const
{
  std::vector<inf::base::external_resource> result;
  auto path = std::filesystem::path(plugin_file).parent_path().parent_path() / "Resources" / "Themes";
  for (auto const& entry : std::filesystem::directory_iterator(path))
  {
    if (!entry.is_directory()) continue;
    external_resource preset;
    preset.path = entry.path().string();
    preset.name = entry.path().filename().string();
    result.push_back(preset);
  }
  return result;
}

std::vector<inf::base::external_resource> 
vst_controller::factory_presets(std::string const& plugin_file) const
{
  std::string dot_extension = ".vstpreset";
  std::vector<inf::base::external_resource> result;
  auto path = std::filesystem::path(plugin_file).parent_path().parent_path() / "Resources" / "Presets";
  if(topology()->is_instrument()) path = path / "Instrument";
  else path = path / "Fx";
  for (auto const& entry: std::filesystem::directory_iterator(path))
  {
    if (entry.path().extension().string() != dot_extension) continue;
    external_resource preset;
    preset.path = entry.path().string();
    preset.name = entry.path().stem().string();
    result.push_back(preset);
  }
  return result;
}

tresult 
vst_controller::setState(IBStream* state)
{
  if(state == nullptr) return kResultFalse;
  IBStreamer streamer(state, kLittleEndian);
  vst_io_stream stream(&streamer);
  stream.load_controller(*topology(), meta_data());
  reloaded();
  return kResultOk;
}

tresult 
vst_controller::getState(IBStream* state)
{
  if (state == nullptr) return kResultFalse;
  IBStreamer streamer(state, kLittleEndian);
  vst_io_stream stream(&streamer);
  stream.save_controller(*topology(), meta_data());
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
  _state[index] = vst_normalized_to_base(_topology.get(), index, normalized);
}

void 
vst_controller::load_component_state(param_value* state)
{
  // SetParamNormalized() each value.
  for (std::int32_t p = 0; p < _topology->input_param_count; p++)
  {
    ParamID tag = _topology->param_index_to_id[p];
    ParamValue value = base_to_vst_normalized(_topology.get(), p, state[p]);
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

// See PresetFile::loadPreset. We load processor (component) state
// from stream into controller, then flush params to processor.
bool
vst_controller::load_preset(std::string const& path, bool factory)
{
  // Load preset format from disk and parse.
  std::ifstream file(path, std::ios::binary | std::ios::ate);
  std::streamsize size = file.tellg();
  file.seekg(0, std::ios::beg);
  std::vector<char> buffer = std::vector<char>(size);
  if (!file.read(buffer.data(), size)) return false;
  MemoryStream memory(buffer.data(), buffer.size());
  PresetFile preset(&memory);  
  if (!preset.readChunkList()) return false;

  // Allow to share factory presets by versioned/unversioned.
  if (!factory && preset.getClassID() != _processor_id) return false;

  // Load controller and processor state. 
  // Controller state is optional, older file format versions dont have it.
  // Dont mess with the ui settings for factory presets.
  if(!factory && preset.seekToControllerState())
    if(setState(&memory) != kResultOk) return false;
  if (!preset.seekToComponentState()) return false;
  return set_component_state(&memory) == kResultOk;
}

// Save using full vstpreset headers. See PresetFile::savePreset. 
// Treat controller state as processor state.
void
vst_controller::save_preset(std::string const& path)
{
  // Dump processor state.
  MemoryStream processor_state;
  IBStreamer processor_streamer(&processor_state, kLittleEndian);
  vst_io_stream processor_stream(&processor_streamer);
  if (!processor_stream.save_processor(*_topology, _state.data())) return;
  if (processor_state.seek(0, IBStream::kIBSeekSet, nullptr) != kResultTrue) return;

  // Dump controller state.
  MemoryStream controller_state;
  IBStreamer controller_streamer(&controller_state, kLittleEndian);
  vst_io_stream controller_stream(&controller_streamer);
  if (!controller_stream.save_controller(*_topology, meta_data())) return;
  if (controller_state.seek(0, IBStream::kIBSeekSet, nullptr) != kResultTrue) return;

  // Dump both plus metadata to preset format.
  MemoryStream preset_state;
  if (!PresetFile::savePreset(&preset_state, _processor_id, &processor_state, &controller_state)) return;
  if (preset_state.seek(0, IBStream::kIBSeekSet, nullptr) != kResultTrue) return;

  // Write preset format to disk.
  std::vector<char> contents(static_cast<std::size_t>(preset_state.getSize()), '0');
  if (preset_state.read(contents.data(), preset_state.getSize(), nullptr) != kResultTrue) return;
  std::ofstream file(path, std::ios::out | std::ios::binary);
  if (file.bad()) return;
  file.write(contents.data(), preset_state.getSize());
  file.close();
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

} // namespace inf::base::vst