#include <inf.vst.ui/ui_controller.hpp>
#include <vstgui/lib/platform/platformfactory.h>
#if WIN32
#include <vstgui/lib/platform/win32/win32factory.h>
#endif
   
using namespace VSTGUI;
using namespace inf::base;
using namespace Steinberg;
using namespace Steinberg::Vst;                  

namespace inf::vst::ui {

vst_ui_controller::
vst_ui_controller(std::unique_ptr<inf::base::topology_info>&& topology) :
vst_controller(std::move(topology)) {}

IPlugView* PLUGIN_API
vst_ui_controller::createView(char const* name)
{
  if (ConstString(name) != ViewType::kEditor) return nullptr;
  vst_editor* result = new vst_editor(this, "view", "UI/controller.uidesc", _topology.get());
  setKnobMode(KnobModes::kLinearMode);
  return result;
}   

// Right-click context menu, init/clear patch.
void
vst_ui_controller::add_patch_items(COptionMenu* menu)
{
  // Init patch to defaults.
  auto init_patch = new CCommandMenuItem(CCommandMenuItem::Desc("Init patch"));
  init_patch->setActions([this](CCommandMenuItem*) { 
    std::vector<param_value> new_values(_topology->input_param_count, param_value());
    _topology->init_factory_preset(new_values.data());
    load_component_state(new_values.data(), true);
  });
  menu->addEntry(init_patch);

  // Clear patch.
  auto clear_patch = new CCommandMenuItem(CCommandMenuItem::Desc("Clear patch"));
  clear_patch->setActions([this](CCommandMenuItem*) {
    std::vector<param_value> new_values(_topology->input_param_count, param_value());
    _topology->init_clear_patch(new_values.data());
    load_component_state(new_values.data(), true);
  });
  menu->addEntry(clear_patch);
}

// Right-click context menu, clear module.
void
vst_ui_controller::add_clear_items(COptionMenu* menu)
{
  std::vector<CCommandMenuItem*> clear_single;
  std::vector<CCommandMenuItem*> clear_multiple;
  auto clear_items = new COptionMenu();
  auto clear_item = new CCommandMenuItem(CCommandMenuItem::Desc("Clear"));
  for (std::int32_t i = 0; i < _topology->static_part_count; i++)
    if (_topology->static_parts[i].kind == part_kind::input)
    {
      auto part_name = _topology->static_parts[i].static_name.short_;
      auto clear_part_item = new CCommandMenuItem(CCommandMenuItem::Desc(part_name));
      if (_topology->static_parts[i].part_count == 1)
      {
        clear_part_item->setActions([=, this](CCommandMenuItem*) { clear_module(i, 0); });
        clear_single.push_back(clear_part_item);
      }
      else
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
  for (std::size_t i = 0; i < clear_single.size(); i++)
    clear_items->addEntry(clear_single[i]);
  for (std::size_t i = 0; i < clear_multiple.size(); i++)
    clear_items->addEntry(clear_multiple[i]);
  clear_item->setSubmenu(clear_items);
  menu->addEntry(clear_item);
}

// Right-click context menu, load preset.
void 
vst_ui_controller::add_preset_select_items(COptionMenu* menu)
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
    for (auto const& entry : std::filesystem::directory_iterator((base_path.value() + "/Presets").data()))
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
vst_ui_controller::add_copy_swap_menu_items(COptionMenu* menu,
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
vst_ui_controller::createContextMenu(CPoint const& pos, VST3Editor* editor)
{
  ParamID tag;
  auto& inf_editor = dynamic_cast<vst_editor&>(*editor);
  tresult found = inf_editor.find_parameter(pos, tag);
  if(found == kResultOk) return VST3EditorDelegate::createContextMenu(pos, editor);

  // Init, clear, copy/swap items, and preset selector. 
  // Just do it manually, kIsProgramChange don't work well.
  auto result = new COptionMenu();
  add_patch_items(result);
  add_clear_items(result);
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
  add_preset_select_items(result);
  return result;
}

} // namespace inf::vst::ui