#ifndef INF_BASE_UI_SHARED_CONFIG_HPP
#define INF_BASE_UI_SHARED_CONFIG_HPP

#include <juce_gui_basics/juce_gui_basics.h>

namespace inf::base::ui {

inline int const container_padding = 2;
inline float const container_outline_size = 1.0f;

inline juce::Point<int> container_selector_padding = { 10, 8 };

inline float const selector_min_height = 26.0f;
inline float const selector_max_height = 42.0f;
inline float const selector_font_min_height = 11.0f;
inline float const selector_font_max_height = 14.0f;

inline float const dropdown_hpad = 4.0f;
inline float const dropdown_vpad_min = 6.0f;
inline float const dropdown_vpad_max = 32.0f;
inline float const dropdown_item_vpad_min = 6.0f;
inline float const dropdown_item_vpad_max = 12.0f;
inline float const dropdown_font_min_height = 10.0f;
inline float const dropdown_font_max_height = 12.0f;

inline float const param_label_vpad = 4.0f;
inline float const param_label_font_min_height = 10.0f;
inline float const param_label_font_max_height = 12.0f;

inline float const group_label_vpad = 7.0f;
inline float const group_label_font_min_height = 11.0f;
inline float const group_label_font_max_height = 14.0f;

inline float const param_label_hslider_min_width = 40.0f;
inline float const param_label_hslider_max_width = 48.0f;

} // namespace inf::base::ui
#endif // INF_BASE_UI_SHARED_CONFIG_HPP