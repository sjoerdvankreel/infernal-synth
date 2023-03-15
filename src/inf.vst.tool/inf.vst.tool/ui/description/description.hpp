#ifndef INF_VST_TOOL_UI_DESCRIPTION_DESCRIPTION_HPP
#define INF_VST_TOOL_UI_DESCRIPTION_DESCRIPTION_HPP

#include <inf.base/topology/topology_info.hpp>
#include <inf.base/topology/part_ui_descriptor.hpp>

#include <vector>
#include <cstdint>
#include <ostream>

namespace inf::vst::tool::ui {

// Grid/table index for single parameter.
struct param_ui_description
{
  std::int32_t row;
  std::int32_t column;
  inf::base::param_info const* info;
};

// Complete part with graph and params.
struct part_ui_description
{
  std::int32_t width;
  std::int32_t height;
  std::int32_t rows; // Grid/table rows.
  std::int32_t columns; // Display column count (either regular or table).
  std::int32_t occupied_cell_count; // Param unique ui indices plus graphs sizes.

  std::int32_t runtime_index;
  bool enabled_param_relevant;
  inf::base::part_info const* info;
  param_ui_description enabled_param;
  std::vector<param_ui_description> params;

  static part_ui_description create(inf::base::topology_info const& topology,
    inf::base::part_info const* info, std::int32_t static_part_index, std::int32_t runtime_index);
};

// Part type container, tabbed if more than 1.
struct part_type_ui_description
{
  std::int32_t top;
  std::int32_t left;
  std::int32_t width;
  std::int32_t height;
  std::int32_t column; // Own column.

  std::string part_name;
  inf::base::part_ui_colors colors; // Type ui colors.
  param_ui_description selector_param; // Osc1/2 etc.
  std::vector<part_ui_description> parts;

  static part_type_ui_description
  create(inf::base::topology_info const& topology, inf::base::part_descriptor const& descriptor,
    std::int32_t runtime_selector_indices_start, std::vector<std::int32_t> runtime_part_indices);
};

// Complete controller editor ui.
struct controller_ui_description
{
  std::int32_t width;
  std::int32_t height;
  std::vector<std::int32_t> column_widths;
  std::vector<part_type_ui_description> part_types;

  static controller_ui_description
  create(inf::base::topology_info const& topology);
};

} // namespace inf::vst::tool::ui
#endif // INF_VST_TOOL_UI_DESCRIPTION_DESCRIPTION_HPP