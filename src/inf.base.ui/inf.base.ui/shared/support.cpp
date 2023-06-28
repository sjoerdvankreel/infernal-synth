#include <inf.base.ui/shared/support.hpp>
#include <juce_gui_basics/juce_gui_basics.h>

#include <cassert>
#include <algorithm>

using namespace juce;
using namespace inf::base;

namespace inf::base::ui {

void
show_host_menu_for_param(base::plugin_controller* controller, std::int32_t param_index, juce::LookAndFeel* lnf)
{
  PopupMenu menu;
  menu.setLookAndFeel(lnf);
  auto host_menu = controller->host_menu_for_param_index(param_index);
  for (std::int32_t i = 0; i < host_menu->item_count(); i++)
    menu.addItem(i + 1, host_menu->item_name(i));
  menu.showMenuAsync(PopupMenu::Options(), [host_menu = host_menu.release()](int option) {
    if (option != 0) host_menu->item_clicked(option - 1);
    delete host_menu;
  });
}

std::string
format_label_short(plugin_controller const* controller, std::int32_t param_index)
{
  auto const& param_info = controller->topology()->params[param_index];
  auto const& part_info = controller->topology()->parts[param_info.part_index];
  auto result = std::string(part_info.descriptor->static_name.short_);
  if(part_info.descriptor->part_count > 1) result += std::string(" ") + std::to_string(part_info.type_index + 1);
  return result + std::string(" ") + param_info.descriptor->data.static_name.short_;
}

std::string
get_label_text(param_descriptor const* descriptor, label_type type, param_value value)
{
  switch (type)
  {
  case label_type::label: return descriptor->data.static_name.short_;
  case label_type::value: return descriptor->data.format(false, value);
  default: assert(false); return {};
  }
}

float
get_scaled_size(plugin_controller const* controller, float min_size, float max_size)
{
  float const current_width = static_cast<float>(controller->editor_current_width());
  float const scale_min_width = static_cast<float>(controller->editor_font_scaling_min_width());
  float const scale_max_width = static_cast<float>(controller->editor_font_scaling_max_width());
  if(current_width <= scale_min_width) return min_size;
  if(current_width >= scale_max_width) return max_size;
  float const factor = (current_width - scale_min_width) / (scale_max_width - scale_min_width);
  float const result = min_size + factor * (max_size - min_size);
  return static_cast<float>(static_cast<std::int32_t>(result));
}

std::int32_t
plugin_editor_width(plugin_controller const* controller, std::int32_t selected_size_index)
{
  selected_size_index = std::clamp(selected_size_index, 0, static_cast<std::int32_t>(controller->ui_size_names().size() - 1));
  float min_width = static_cast<float>(controller->editor_min_width());
  float max_width = static_cast<float>(controller->editor_max_width());
  float factor = static_cast<float>(selected_size_index) / static_cast<float>(controller->ui_size_names().size() - 1);
  return static_cast<std::int32_t>(min_width + factor * (max_width - min_width));
}

} // namespace inf::base::ui