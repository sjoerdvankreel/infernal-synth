// Some VST header missing an include.
#include <cassert>

#include <base/source/fstring.h>
#include <base/source/fstreamer.h>
#include <pluginterfaces/base/ibstream.h>
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
vst_controller::set_editor_width(std::int32_t width)
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
  return (path.parent_path().parent_path() / "Resources" / "Themes" / "Default").string();
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
vst_controller::set_component_state(IBStream* state)
{
  if (state == nullptr) return kResultFalse;

  // Load state into temporary buffer.
  IBStreamer streamer(state, kLittleEndian);
  vst_io_stream stream(&streamer);
  std::vector<param_value> values(_topology->input_param_count, param_value());
  if (!stream.load(*_topology, values.data(), meta_data())) return kResultFalse;
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
  reloaded();
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
  if (!preset.seekToComponentState()) return false;
  return set_component_state(&memory) == kResultOk;
}

// Save using full vstpreset headers. See PresetFile::savePreset. 
// Treat controller state as processor state.
void
vst_controller::save_preset(std::string const& path)
{
  MemoryStream preset_state;
  MemoryStream processor_state;
  IBStreamer streamer(&processor_state, kLittleEndian);
  vst_io_stream stream(&streamer);
  if (!stream.save(*_topology, _state.data(), meta_data())) return;
  if (processor_state.seek(0, IBStream::kIBSeekSet, nullptr) != kResultTrue) return;
  if (!PresetFile::savePreset(&preset_state, _processor_id, &processor_state)) return;
  if (preset_state.seek(0, IBStream::kIBSeekSet, nullptr) != kResultTrue) return;
  std::vector<char> contents(static_cast<std::size_t>(preset_state.getSize()), '0');
  if (preset_state.read(contents.data(), preset_state.getSize(), nullptr) != kResultTrue) return;
  std::ofstream file(path, std::ios::out | std::ios::binary);
  if (file.bad()) return;
  file.write(contents.data(), preset_state.getSize());
  file.close();
}

} // namespace inf::base::vst