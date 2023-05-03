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
enum class edit_type { knob, selector, hslider, vslider, toggle, dropdown };

float
get_text_font_height(plugin_controller const* controller, float min_height, float max_height);
std::string
get_label_text(base::param_descriptor const* descriptor, label_type type, base::param_value value);

inline float
get_param_label_font_height(inf::base::plugin_controller const* controller)
{ return get_text_font_height(controller, param_label_font_min_height, param_label_font_max_height); }
inline float
get_param_label_total_height(inf::base::plugin_controller const* controller)
{ return get_param_label_font_height(controller) + param_label_vpad; }

inline float
get_group_label_font_height(inf::base::plugin_controller const* controller)
{ return get_text_font_height(controller, group_label_font_min_height, group_label_font_max_height); }
inline float
get_group_label_total_height(inf::base::plugin_controller const* controller)
{ return get_group_label_font_height(controller) + group_label_vpad; }

} // namespace inf::base::ui
#endif // INF_BASE_UI_SHARED_SUPPORT_HPP