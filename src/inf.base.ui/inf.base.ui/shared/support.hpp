#ifndef INF_BASE_UI_SHARED_SUPPORT_HPP
#define INF_BASE_UI_SHARED_SUPPORT_HPP

#include <inf.base/plugin/plugin_controller.hpp>
#include <inf.base/shared/state.hpp>
#include <inf.base/topology/topology_info.hpp>
#include <inf.base.ui/shared/config.hpp>

#include <string>
#include <cstdint>

namespace inf::base::ui {

enum class label_type { label, value };
enum class tooltip_type { off, label, value };
enum class icon_type { sine, saw, tri, pulse };
enum class edit_type { knob, selector, hslider, vslider, toggle, dropdown, tab_bar };

std::string
format_label_short(plugin_controller const* controller, std::int32_t param_index);
float
get_scaled_size(plugin_controller const* controller, float min_size, float max_size);
std::string
get_label_text(base::param_descriptor const* descriptor, label_type type, base::param_value value);

inline float
get_toggle_max_size(inf::base::plugin_controller const* controller)
{ return get_scaled_size(controller, toggle_max_size_min, toggle_max_size_max); }
inline float
get_text_edit_font_height(inf::base::plugin_controller const* controller)
{ return get_scaled_size(controller, text_edit_font_min_height, text_edit_font_max_height); }

inline float
get_selector_height(inf::base::plugin_controller const* controller)
{ return get_scaled_size(controller, selector_min_height, selector_max_height); }
inline float
get_selector_vlabel_pad(inf::base::plugin_controller const* controller)
{ return get_scaled_size(controller, selector_vlabel_pad_min, selector_vlabel_pad_max); }
inline float
get_selector_font_height(inf::base::plugin_controller const* controller)
{ return get_scaled_size(controller, selector_font_min_height, selector_font_max_height); }

inline float
get_dropdown_font_height(inf::base::plugin_controller const* controller)
{ return get_scaled_size(controller, dropdown_font_min_height, dropdown_font_max_height); }
inline float
get_dropdown_vpad(inf::base::plugin_controller const* controller)
{ return get_scaled_size(controller, dropdown_vpad_min, dropdown_vpad_max); }
inline float
get_dropdown_height(inf::base::plugin_controller const* controller)
{ return get_dropdown_font_height(controller) + get_dropdown_vpad(controller); }
inline float
get_dropdown_item_vpad(inf::base::plugin_controller const* controller)
{ return get_scaled_size(controller, dropdown_item_vpad_min, dropdown_item_vpad_max); }
inline float
get_dropdown_item_height(inf::base::plugin_controller const* controller)
{ return get_dropdown_font_height(controller) + get_dropdown_item_vpad(controller); }

inline float
get_param_label_font_height(inf::base::plugin_controller const* controller)
{ return get_scaled_size(controller, param_label_font_min_height, param_label_font_max_height); }
inline float
get_param_label_total_height(inf::base::plugin_controller const* controller)
{ return get_param_label_font_height(controller) + param_label_vpad; }

inline float
get_group_label_font_height(inf::base::plugin_controller const* controller)
{ return get_scaled_size(controller, group_label_font_min_height, group_label_font_max_height); }
inline float
get_group_label_total_height(inf::base::plugin_controller const* controller)
{ return get_group_label_font_height(controller) + group_label_vpad; }

inline float
get_file_browser_font_height(inf::base::plugin_controller const* controller)
{ return get_scaled_size(controller, file_browser_font_min_height, file_browser_font_max_height); }
inline float
get_button_label_font_height(inf::base::plugin_controller const* controller)
{ return get_scaled_size(controller, button_label_font_min_height, button_label_font_max_height); }

} // namespace inf::base::ui
#endif // INF_BASE_UI_SHARED_SUPPORT_HPP