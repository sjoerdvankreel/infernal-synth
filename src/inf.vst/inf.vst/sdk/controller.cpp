// Some VST header missing an include.
#include <cassert>

#include <base/source/fstreamer.h>
#include <pluginterfaces/base/ibstream.h>
#include <public.sdk/source/vst/vstpresetfile.h>

// Why? Is it missing from vst's cmake?
// In any case, can't link without it.
#include <public.sdk/source/common/memorystream.h>
#include <public.sdk/source/common/memorystream.cpp>

#include <inf.base/shared/support.hpp>
#include <inf.vst/sdk/parameter.hpp>
#include <inf.vst/sdk/controller.hpp>
#include <inf.vst/shared/support.hpp>
#include <inf.vst/shared/io_stream.hpp>

#include <vector>
#include <cstring>
#include <cstdint>
#include <fstream>

using namespace inf::base;
using namespace Steinberg;
using namespace Steinberg::Vst;                  

namespace inf::vst {

vst_controller::
vst_controller(std::unique_ptr<inf::base::topology_info>&& topology) :
_state(topology->params.size()), _preset_items(), _topology(std::move(topology))
{ _topology->init_factory_preset(_state.data()); }

void
vst_controller::load_preset(std::size_t index)
{
  if (index >= _preset_items.size()) return;
  std::string path = _preset_items[index].path;
  std::ifstream file(path, std::ios::binary | std::ios::ate);
  std::streamsize size = file.tellg();
  file.seekg(0, std::ios::beg);
  std::vector<char> buffer(size);
  if (!file.read(buffer.data(), size)) return;
  MemoryStream memory(buffer.data(), buffer.size());
  PresetFile preset(&memory);
  if (!preset.readChunkList()) return;
  if (!preset.seekToComponentState()) return;
  set_component_state(&memory, true);
}

void
vst_controller::sync_ui_parameters()
{
  // Updates visibility of dependent parameters and rendering of graphs.
  for (std::size_t p = 0; p < _topology->params.size(); p++)
    endEdit(_topology->param_index_to_id[static_cast<std::int32_t>(p)]);
}

tresult
vst_controller::set_component_state(IBStream* state, bool perform_edit)
{
  if (state == nullptr) return kResultFalse;

  // Load state into temporary buffer.
  IBStreamer streamer(state, kLittleEndian);
  vst_io_stream stream(&streamer);
  std::vector<param_value> values(_topology->input_param_count, param_value());
  if (!stream.load(*_topology, values.data())) return kResultFalse;
  load_component_state(values.data(), perform_edit);
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
vst_controller::load_component_state(param_value* state, bool perform_edit)
{
  // SetParamNormalized() each value.
  for (std::int32_t p = 0; p < _topology->input_param_count; p++)
  {
    ParamID tag = _topology->param_index_to_id[p];
    ParamValue value = base_to_vst_normalized(_topology.get(), p, state[p]);
    if (!perform_edit)
      setParamNormalized(tag, value);
    else
    {
      beginEdit(tag);
      setParamNormalized(tag, value);
      performEdit(tag, value);
      endEdit(tag);
    }
    update_state(tag);
  }

  // Set initial ui visibility state.
  sync_ui_parameters();
}

tresult PLUGIN_API
vst_controller::initialize(FUnknown* context)
{
  tresult result = EditControllerEx1::initialize(context);
  if (result != kResultTrue) return result;

  // Add parts as units.
  for (std::size_t p = 0; p < _topology->parts.size(); p++)
    addUnit(new Unit(
      to_vst_string(_topology->parts[p].runtime_name.c_str()).c_str(), 
      static_cast<Steinberg::int32>(p + 1), kRootUnitId));
  // Add all runtime parameters.
  for (std::int32_t p = 0; p < static_cast<std::int32_t>(_topology->params.size()); p++)
  {
    ParamID tag = _topology->param_index_to_id[p];
    part_info const* part = &_topology->parts[_topology->params[p].part_index];
    parameters.addParameter(new vst_parameter(tag, part, &_topology->params[p]));
  }
  return kResultTrue;
}

} // namespace inf::vst