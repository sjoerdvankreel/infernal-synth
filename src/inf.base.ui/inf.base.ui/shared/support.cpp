#include <inf.base.ui/shared/support.hpp>
#include <vstgui4/vstgui/lib/controls/coptionmenu.h>
#include <vstgui/lib/platform/platformfactory.h>
#if WIN32
#include <vstgui/lib/platform/win32/win32factory.h>
#endif

#include <filesystem>

using namespace VSTGUI;

namespace inf::base::ui {

inf::base::ui_color
from_vst_color_name(std::string const* name, IUIDescription const* desc)
{
  CColor color;
  assert(name != nullptr);
  bool ok = UIViewCreator::stringToColor(name, color, desc);
  (void)ok;
  assert(ok);
  return from_vst_color(color);
}

static void
clear_module(plugin_controller* controller, std::int32_t type, std::int32_t index)
{
  param_value const* state = controller->state();
  topology_info const* topology = controller->topology();
  std::int32_t param_start = topology->param_bounds[type][index];
  std::int32_t param_count = topology->static_parts[type].param_count;
  std::vector<param_value> new_values(state, state + topology->params.size());
  topology->init_param_defaults(new_values.data(), param_start, param_start + param_count);
  controller->load_component_state(new_values.data(), true);
}

static CMenuItem*
create_copy_swap_context_menu(plugin_controller* controller,
  std::string const& action, std::string const& target_prefix,
  std::function<void(std::int32_t, std::int32_t, std::int32_t)> apply)
{
  topology_info const* topology = controller->topology();
  auto item = new CCommandMenuItem(CCommandMenuItem::Desc(action.c_str()));
  auto static_part_menu = new COptionMenu();
  for (std::int32_t i = 0; i < topology->static_part_count; i++)
    if (topology->static_parts[i].part_count > 1)
    {
      auto part_name = topology->static_parts[i].static_name.short_;
      auto static_part_item = new CCommandMenuItem(CCommandMenuItem::Desc(part_name));
      static_part_menu->addEntry(static_part_item);
      auto rt_part_menu_source = new COptionMenu();
      for (std::int32_t j = 0; j < topology->static_parts[i].part_count; j++)
      {
        std::int32_t rt_source_index = topology->part_bounds[i][j];
        auto source_name = topology->parts[rt_source_index].runtime_name;
        auto rt_part_item_source = new CCommandMenuItem(CCommandMenuItem::Desc(source_name.c_str()));
        rt_part_menu_source->addEntry(rt_part_item_source);
        auto rt_part_menu_target = new COptionMenu();
        for (std::int32_t k = 0; k < topology->static_parts[i].part_count; k++)
          if (k != j)
          {
            std::int32_t rt_target_index = topology->part_bounds[i][k];
            auto target_name = std::string(target_prefix) + " " + topology->parts[rt_target_index].runtime_name;
            auto rt_part_item_target = new CCommandMenuItem(CCommandMenuItem::Desc(target_name.c_str()));
            rt_part_menu_target->addEntry(rt_part_item_target);
            rt_part_item_target->setActions([=](CCommandMenuItem*) {
              std::int32_t source_begin = topology->param_bounds[i][j];
              std::int32_t target_begin = topology->param_bounds[i][k];
              std::int32_t param_count = topology->static_parts[i].param_count;
              apply(source_begin, target_begin, param_count);
              controller->restart();
            });
          }
        rt_part_item_source->setSubmenu(rt_part_menu_target);
      }
      static_part_item->setSubmenu(rt_part_menu_source);
    }
  item->setSubmenu(static_part_menu);
  return item;
}

std::vector<VSTGUI::CMenuItem*>
create_context_menu(plugin_controller* controller)
{
  std::vector<VSTGUI::CMenuItem*> result;
  topology_info const* topology = controller->topology();

  // Init patch to defaults.
  auto init_patch = new CCommandMenuItem(CCommandMenuItem::Desc("Init patch"));
  init_patch->setActions([controller](CCommandMenuItem*) {
    std::vector<param_value> new_values(controller->topology()->input_param_count, param_value());
    controller->topology()->init_factory_preset(new_values.data());
    controller->load_component_state(new_values.data(), true);
  });
  result.push_back(init_patch);

  // Clear patch.
  auto clear_patch = new CCommandMenuItem(CCommandMenuItem::Desc("Clear patch"));
  clear_patch->setActions([controller](CCommandMenuItem*) {
    std::vector<param_value> new_values(controller->topology()->input_param_count, param_value());
    controller->topology()->init_clear_patch(new_values.data());
    controller->load_component_state(new_values.data(), true);
  });
  result.push_back(clear_patch);

  // Clear single module.
  std::vector<CCommandMenuItem*> clear_single;
  std::vector<CCommandMenuItem*> clear_multiple;
  auto clear_items = new COptionMenu();
  auto clear_item = new CCommandMenuItem(CCommandMenuItem::Desc("Clear"));
  for (std::int32_t i = 0; i < topology->static_part_count; i++)
  {
    if (topology->static_parts[i].kind != part_kind::input) continue;
    auto part_name = topology->static_parts[i].static_name.short_;
    auto clear_part_item = new CCommandMenuItem(CCommandMenuItem::Desc(part_name));
    if (topology->static_parts[i].part_count == 1)
    {
      clear_part_item->setActions([controller, i](CCommandMenuItem*) { clear_module(controller, i, 0); });
      clear_single.push_back(clear_part_item);
    }
    else
    {
      auto clear_part_items = new COptionMenu();
      for (std::int32_t j = 0; j < topology->static_parts[i].part_count; j++)
      {
        std::int32_t rt_part_index = topology->part_bounds[i][j];
        auto target_name = topology->parts[rt_part_index].runtime_name;
        auto clear_rt_part_item = new CCommandMenuItem(CCommandMenuItem::Desc(target_name.c_str()));
        clear_rt_part_item->setActions([controller, i, j](CCommandMenuItem*) { clear_module(controller, i, j); });
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
  result.push_back(clear_item);

  // Copy module to.
  result.push_back(create_copy_swap_context_menu(controller, "Copy", "To",
    [controller](std::int32_t source_begin, std::int32_t target_begin, std::int32_t param_count) {
      for (std::int32_t i = 0; i < param_count; i++)
      {
        std::int32_t source_tag = controller->topology()->param_index_to_id[source_begin + i];
        std::int32_t target_tag = controller->topology()->param_index_to_id[target_begin + i];
        controller->copy_param(source_tag, target_tag);
      }}));

  // Swap module with.
  result.push_back(create_copy_swap_context_menu(controller, "Swap", "With",
    [controller](std::int32_t source_begin, std::int32_t target_begin, std::int32_t param_count) {
      for (std::int32_t i = 0; i < param_count; i++)
      {
        std::int32_t source_tag = controller->topology()->param_index_to_id[source_begin + i];
        std::int32_t target_tag = controller->topology()->param_index_to_id[target_begin + i];
        controller->swap_param(source_tag, target_tag);
      }}));

  // Scan directory only once.
  if (controller->preset_items().size() == 0)
  {
    VSTGUI::Optional<VSTGUI::UTF8String> base_path = {};
    // I really want my GCC -WALL compilation. Has saved me more than once.
#if WIN32
    auto const* factory = getPlatformFactory().asWin32Factory();
    if (factory == nullptr) return result;
    base_path = factory->getResourceBasePath();
#endif
    if (!base_path) return result;
    for (auto const& entry : std::filesystem::directory_iterator((base_path.value() + "/Presets").data()))
    {
      if (entry.path().extension().string() != ".vstpreset") continue;
      preset_item item;
      item.path = entry.path().string();
      item.name = entry.path().stem().string();
      controller->preset_items().push_back(item);
    }
  }

  // Add submenu with presets.
  if (controller->preset_items().size() == 0) return result;
  auto load_item = new CCommandMenuItem(CCommandMenuItem::Desc("Load preset"));
  auto preset_menu = new COptionMenu();
  for (std::size_t i = 0; i < controller->preset_items().size(); i++)
  {
    auto preset_item = new CCommandMenuItem(CCommandMenuItem::Desc(controller->preset_items()[i].name.c_str()));
    preset_menu->addEntry(preset_item);
    preset_item->setActions([controller, i](CCommandMenuItem*) { controller->load_preset(i); });
  }
  load_item->setSubmenu(preset_menu);
  result.push_back(load_item);

  return result;
}

} // namespace inf::base::ui