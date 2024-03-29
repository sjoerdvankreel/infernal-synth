#ifndef INF_BASE_UI_SHARED_CONFIG_HPP
#define INF_BASE_UI_SHARED_CONFIG_HPP

#include <juce_gui_basics/juce_gui_basics.h>

namespace inf::base::ui {

inline juce::Point<int> container_part_padding = { 2, 2 };
inline juce::Point<int> container_selector_padding = { 10, 8 };

inline int const edit_margin = 2;
inline int const button_margin = 3;
inline int const dropdown_margin = 2;
inline int const container_margin = 2;
inline int const edit_table_margin = 0;
inline int const graph_grid_hcount = 9;

inline float const router_width_min = 16.0f;
inline float const router_width_max = 24.0f;
inline float const router_line_size_min = 4.0f;
inline float const router_line_size_max = 6.0f;
inline float const router_arrow_size_min = 8.0f;
inline float const router_arrow_size_max = 12.0f;

inline float const container_radius = 8.0f;
inline float const container_outline_size = 1.0f;
inline float const dialog_font_height = 13.0f;
inline float const dialog_font_header_height = 15.0f;

inline float const toggle_max_size_min = 18.0f;
inline float const toggle_max_size_max = 34.0f;

inline float const text_edit_font_min_height = 11.0f;
inline float const text_edit_font_max_height = 15.0f;

inline float const selector_min_height = 26.0f;
inline float const selector_max_height = 40.0f;
inline float const selector_vlabel_pad_min = 2.0f;
inline float const selector_vlabel_pad_max = 6.0f;
inline float const selector_font_min_height = 11.0f;
inline float const selector_font_max_height = 15.0f;

inline float const dropdown_hpad = 4.0f;
inline float const dropdown_vpad_min = 6.0f;
inline float const dropdown_vpad_max = 17.0f;
inline float const dropdown_item_vpad_min = 6.0f;
inline float const dropdown_item_vpad_max = 9.0f;
inline float const dropdown_font_min_height = 10.0f;
inline float const dropdown_font_max_height = 14.0f;
inline int   const dropdown_max_column_items = 20;

inline float const param_label_vpad = 4.0f;
inline float const param_label_font_min_height = 10.0f;
inline float const param_label_font_max_height = 14.0f;

inline float const button_label_font_min_height = 11.0f;
inline float const button_label_font_max_height = 15.0f;

inline float const group_label_vpad = 7.0f;
inline float const group_label_font_min_height = 11.0f;
inline float const group_label_font_max_height = 15.0f;

inline float const file_browser_font_min_height = 11.0f;
inline float const file_browser_font_max_height = 15.0f;

} // namespace inf::base::ui
#endif // INF_BASE_UI_SHARED_CONFIG_HPP