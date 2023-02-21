// Some VST header missing an include.
#include <cassert>

#include <base/source/fstreamer.h>
#include <pluginterfaces/base/ibstream.h>
#include <public.sdk/source/vst/vstpresetfile.h>
#include <vstgui/lib/platform/platformfactory.h>
#if WIN32
#include <vstgui/lib/platform/win32/win32factory.h>
#endif

// Why? Is it missing from vst's cmake?
// In any case, can't link without it.
#include <public.sdk/source/common/memorystream.h>
#include <public.sdk/source/common/memorystream.cpp>

#include <inf.base/shared/support.hpp>
#include <inf.vst.base/ui/vst_editor.hpp>
#include <inf.vst.base/sdk/parameter.hpp>
#include <inf.vst.base/sdk/controller.hpp>
#include <inf.vst.base/shared/support.hpp>
#include <inf.vst.base/shared/io_stream.hpp>

#include <vector>
#include <cstring>
#include <cstdint>
#include <fstream>
#include <filesystem>
   
using namespace VSTGUI;
using namespace inf::base;
using namespace Steinberg;
using namespace Steinberg::Vst;                  

namespace inf::vst::base {

void
vst_controller::sync_ui_parameters()
{
  // Updates visibility of dependent parameters and rendering of graphs.
  for (std::size_t p = 0; p < _topology->params.size(); p++)
    endEdit(_topology->param_index_to_id[static_cast<std::int32_t>(p)]);
}

tresult
vst_controller::endEdit(ParamID tag)
{
  update_state(tag);
  if (_editor == nullptr) return EditControllerEx1::endEdit(tag);

  // Update visibility of dependent parameters and rerender graphs.
  _editor->update_dependent_visibility(tag);
  return EditControllerEx1::endEdit(tag);
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
vst_controller::view_removed(vst_editor* editor)
{ 
  assert(editor != nullptr);
  assert(_editor != nullptr);
  _editor = nullptr; 
}

void 
vst_controller::view_attached(vst_editor* editor)
{ 
  assert(editor != nullptr);
  assert(_editor == nullptr);
  _editor = editor; 
  sync_ui_parameters();
}

IPlugView* PLUGIN_API
vst_controller::createView(char const* name)
{
  if (ConstString(name) != ViewType::kEditor) return nullptr;
  return new vst_editor(this, "view", "controller.uidesc", _topology.get());
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

void 
vst_controller::load_preset(std::size_t index)
{
  if(index < 0 || index >= _preset_items.size()) return;
  std::string path = _preset_items[index].path;
  std::ifstream file(path, std::ios::binary | std::ios::ate);
  std::streamsize size = file.tellg();
  file.seekg(0, std::ios::beg);
  std::vector<char> buffer(size);
  if (!file.read(buffer.data(), size)) return;
  MemoryStream memory(buffer.data(), buffer.size());
  PresetFile preset(&memory);
  if(!preset.readChunkList()) return;
  if(!preset.seekToComponentState()) return;
  set_component_state(&memory, true);
}

void 
vst_controller::clear_module(std::int32_t type, std::int32_t index)
{
  std::vector<param_value> new_values(_state.begin(), _state.end());
  std::int32_t param_start = _topology->param_bounds[type][index];
  std::int32_t param_count = _topology->static_parts[type].param_count;
  _topology->init_defaults(new_values.data(), param_start, param_start + param_count);
  load_component_state(new_values.data(), true);
}

// Right-click context menu, init patch/module.
void
vst_controller::add_init_items(COptionMenu* menu)
{
  // Init patch to defaults.
  auto init_patch = new CCommandMenuItem(CCommandMenuItem::Desc("Init patch"));
  init_patch->setActions([this](CCommandMenuItem*) { 
    std::vector<param_value> new_values(_topology->input_param_count, param_value());
    _topology->init_defaults(new_values.data());
    load_component_state(new_values.data(), true);
  });
  menu->addEntry(init_patch);

  // Clear modules.
  std::vector<CCommandMenuItem*> clear_single;
  std::vector<CCommandMenuItem*> clear_multiple;
  auto clear_items = new COptionMenu();
  auto clear_item = new CCommandMenuItem(CCommandMenuItem::Desc("Clear"));
  for (std::int32_t i = 0; i < _topology->static_part_count; i++)
    if(_topology->static_parts[i].kind == part_kind::input)
    {
      auto part_name = _topology->static_parts[i].static_name.short_;
      auto clear_part_item = new CCommandMenuItem(CCommandMenuItem::Desc(part_name));
      if(_topology->static_parts[i].part_count == 1)
      {
        clear_part_item->setActions([=, this](CCommandMenuItem*) { clear_module(i, 0); });
        clear_single.push_back(clear_part_item);
      } else 
      {
        auto clear_part_items = new COptionMenu();
        for (std::int32_t j = 0; j < _topology->static_parts[i].part_count; j++)
        {
          std::int32_t rt_part_index = _topology->part_bounds[i][j];
          auto target_name = _topology->parts[rt_part_index].runtime_name;
          auto clear_rt_part_item = new CCommandMenuItem(CCommandMenuItem::Desc(target_name.c_str()));
          clear_rt_part_item->setActions([=, this](CCommandMenuItem*) { clear_module(i, j); });
          clear_part_items->addEntry(clear_rt_part_item);
        }
        clear_part_item->setSubmenu(clear_part_items);
        clear_multiple.push_back(clear_part_item);
      }
    }
  for(std::size_t i = 0; i < clear_single.size(); i++)
    clear_items->addEntry(clear_single[i]);
  for (std::size_t i = 0; i < clear_multiple.size(); i++)
    clear_items->addEntry(clear_multiple[i]);
  clear_item->setSubmenu(clear_items);
  menu->addEntry(clear_item);
}

// Right-click context menu, load preset.
void 
vst_controller::add_preset_select_items(COptionMenu* menu)
{
  // Scan directory only once.
  if(!_preset_items_initialized)
  {
    _preset_items_initialized = true;
    VSTGUI::Optional<VSTGUI::UTF8String> base_path = {};
// I really want my GCC -WALL compilation. Has saved me more than once.
#if WIN32
    auto const* factory = getPlatformFactory().asWin32Factory();
    if(factory == nullptr) return;
    base_path = factory->getResourceBasePath();
#endif
    if(!base_path) return;
    for (auto const& entry : std::filesystem::directory_iterator(base_path.value().data()))
    {
      if(entry.path().extension().string() != ".vstpreset") continue;
      preset_item item;
      item.path = entry.path().string();
      item.name = entry.path().stem().string();
      _preset_items.push_back(item);
    }
  }

  // Add submenu with presets.
  if(_preset_items.size() == 0) return;
  auto item = new CCommandMenuItem(CCommandMenuItem::Desc("Load preset"));
  auto preset_menu = new COptionMenu();
  for (std::size_t i = 0; i < _preset_items.size(); i++)
  {
    auto preset_item = new CCommandMenuItem(CCommandMenuItem::Desc(_preset_items[i].name.c_str()));
    preset_menu->addEntry(preset_item);
    preset_item->setActions([=, this](CCommandMenuItem*) { load_preset(i); });
  }
  item->setSubmenu(preset_menu);
  menu->addEntry(item);
}

// Right-click context menu, copy to / swap with.
void 
vst_controller::add_copy_swap_menu_items(COptionMenu* menu, 
  std::string const& action, std::string const& target_prefix,
  std::function<void(std::int32_t, std::int32_t, std::int32_t)> apply)
{
  auto item = new CCommandMenuItem(CCommandMenuItem::Desc(action.c_str()));
  auto static_part_menu = new COptionMenu();
  for (std::int32_t i = 0; i < _topology->static_part_count; i++)
    if(_topology->static_parts[i].part_count > 1)
    {
      auto part_name = _topology->static_parts[i].static_name.short_;
      auto static_part_item = new CCommandMenuItem(CCommandMenuItem::Desc(part_name));
      static_part_menu->addEntry(static_part_item);
      auto rt_part_menu_source = new COptionMenu();
      for (std::int32_t j = 0; j < _topology->static_parts[i].part_count; j++)
      {
        std::int32_t rt_source_index = _topology->part_bounds[i][j];
        auto source_name = _topology->parts[rt_source_index].runtime_name;
        auto rt_part_item_source = new CCommandMenuItem(CCommandMenuItem::Desc(source_name.c_str()));
        rt_part_menu_source->addEntry(rt_part_item_source);
        auto rt_part_menu_target = new COptionMenu();
        for (std::int32_t k = 0; k < _topology->static_parts[i].part_count; k++)
          if(k != j)
          {
            std::int32_t rt_target_index = _topology->part_bounds[i][k];
            auto target_name = std::string(target_prefix) + " " + _topology->parts[rt_target_index].runtime_name;
            auto rt_part_item_target = new CCommandMenuItem(CCommandMenuItem::Desc(target_name.c_str()));
            rt_part_menu_target->addEntry(rt_part_item_target);
            rt_part_item_target->setActions([=, this](CCommandMenuItem*) {
              std::int32_t source_begin = _topology->param_bounds[i][j];
              std::int32_t target_begin = _topology->param_bounds[i][k];
              std::int32_t param_count = _topology->static_parts[i].param_count;
              apply(source_begin, target_begin, param_count);
              componentHandler->restartComponent(kParamValuesChanged);
            });
          }
        rt_part_item_source->setSubmenu(rt_part_menu_target);
      }
      static_part_item->setSubmenu(rt_part_menu_source);
    }
  item->setSubmenu(static_part_menu);
  menu->addEntry(item);
}

// Add copy/switch functionality and load-preset to context menu. 
// Unless user clicked an actual control, then we MUST popup the
// host context menu and cannot augment it otherwise some hosts
// freak out (bitwig, fruity).
COptionMenu* 
vst_controller::createContextMenu(CPoint const& pos, VST3Editor* editor)
{
  ParamID tag;
  auto& inf_editor = dynamic_cast<vst_editor&>(*editor);
  tresult found = inf_editor.find_parameter(pos, tag);
  if(found == kResultOk) return VST3EditorDelegate::createContextMenu(pos, editor);

  // Init, copy/swap items, and preset selector. 
  // Just do it manually, kIsProgramChange don't work well.
  auto result = new COptionMenu();
  add_init_items(result);
  add_preset_select_items(result);
  add_copy_swap_menu_items(result, "Copy", "To",
    [this](std::int32_t source_begin, std::int32_t target_begin, std::int32_t param_count) {
      for (std::int32_t i = 0; i < param_count; i++)
      {
        ParamID source_tag = _topology->param_index_to_id[source_begin + i];
        ParamID target_tag = _topology->param_index_to_id[target_begin + i];
        beginEdit(target_tag);
        setParamNormalized(target_tag, getParamNormalized(source_tag));
        performEdit(target_tag, getParamNormalized(source_tag));
        endEdit(target_tag);
      }});
  add_copy_swap_menu_items(result, "Swap", "With",
    [this](std::int32_t source_begin, std::int32_t target_begin, std::int32_t param_count) {
      for (std::int32_t i = 0; i < param_count; i++)
      {
        ParamID source_tag = _topology->param_index_to_id[source_begin + i];
        ParamID target_tag = _topology->param_index_to_id[target_begin + i];
        ParamValue target = getParamNormalized(target_tag);
        beginEdit(target_tag);
        setParamNormalized(target_tag, getParamNormalized(source_tag));
        performEdit(target_tag, getParamNormalized(source_tag));
        endEdit(target_tag);
        beginEdit(source_tag);
        setParamNormalized(source_tag, target);
        performEdit(source_tag, target);
        endEdit(source_tag);
      }});
  return result;
}

} // namespace inf::vst::base