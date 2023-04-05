#ifndef INF_TOOL_VST_UI_DESCRIPTION_SUPPORT_HPP
#define INF_TOOL_VST_UI_DESCRIPTION_SUPPORT_HPP

#include <inf.base/topology/topology_info.hpp>
#include <inf.base/topology/part_ui_descriptor.hpp>

#include <cstdint>
#include <cstdlib>

namespace inf::tool::vst::ui {

inline inf::base::ui_color constexpr transparent(0x00FFFFFF);
  
inline std::int32_t constexpr checkbox_width = 14;
inline std::int32_t constexpr param_row_height = 21;
inline std::int32_t constexpr param_col1_width = 22;
inline std::int32_t constexpr param_col2_width = 35;
inline std::int32_t constexpr param_col3_width = 40;
inline std::int32_t constexpr header_connector_size = 21;

inline std::int32_t constexpr margin = 3;
inline std::int32_t constexpr padding_param_group = 1;
inline std::int32_t constexpr param_total_width = param_col1_width + margin + param_col2_width + margin + param_col3_width;
inline std::int32_t constexpr param_output_col_width = (param_total_width - margin) / 2;

} // namespace inf::tool::vst::ui
#endif // INF_TOOL_VST_UI_DESCRIPTION_SUPPORT_HPP