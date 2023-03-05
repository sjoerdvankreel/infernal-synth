#ifndef INF_BASE_TOPOLOGY_PART_UI_DESCRIPTOR_HPP
#define INF_BASE_TOPOLOGY_PART_UI_DESCRIPTOR_HPP

#include <cstdint>
#include <vector>

// For ui generator.
// Most stuff can't be const because of ui generator.
namespace inf::base {

// Box coordinates.
struct box_descriptor
{
  std::int32_t row; // Rows in ui grid.
  std::int32_t column; // Columns in ui grid.
  std::int32_t row_span; // Row count in ui grid.
  std::int32_t column_span; // Column count in ui grid.
};

// Optional graph(s) within 1 part.
struct graph_descriptor
{
  std::int32_t type; // Graph type.
  char const* description;
  box_descriptor box;
};

// For arrows between parts.
struct connector_direction_t { enum value { 
  none = 0x0, first = 0x1, up = 0x1, down = 0x2, left = 0x4, right = 0x8, 
  left_right = 0x10, halfway_right_down = 0x20, halfway_left_up = 0x40, last = 0x40 }; };
typedef connector_direction_t::value connector_direction;

// ARGB color.
struct ui_color
{
  std::uint8_t a, r, g, b;

  ui_color() = default;
  constexpr ui_color(std::uint32_t argb):
    a(static_cast<std::uint8_t>((argb & 0xFF000000) >> 24)),
    r(static_cast<std::uint8_t>((argb & 0xFF0000) >> 16)), 
    g(static_cast<std::uint8_t>((argb & 0xFF00) >> 8)), 
    b(static_cast<std::uint8_t>(argb)) {}
  constexpr ui_color(std::uint8_t r, std::uint8_t g, std::uint8_t b) : 
  a(0xFF), r(r), g(g), b(b) {}
  constexpr ui_color(std::uint8_t a, std::uint8_t r, std::uint8_t g, std::uint8_t b) : 
  a(a), r(r), g(g), b(b) {}

  // Note: this is an override.
  constexpr ui_color alpha(std::uint8_t alpha) const
  { return ui_color(alpha, r, g, b); }

  // Note: this is a multiplier.
  constexpr ui_color opacity(float opacity) const
  {
    float new_alphaf = static_cast<float>(a) / 255.0f * opacity;
    std::uint8_t new_alpha = static_cast<std::uint8_t>(new_alphaf * 255.0f);
    return ui_color(new_alpha, r, g, b);
  }

  // 1 = same, 0 = black.
  constexpr ui_color darken(float factor) const
  {
    std::uint8_t new_r = static_cast<std::uint8_t>(r * factor);
    std::uint8_t new_g = static_cast<std::uint8_t>(g * factor);
    std::uint8_t new_b = static_cast<std::uint8_t>(b * factor);
    return ui_color(a, new_r, new_g, new_b);
  }
};

// Shared color combinations.
struct box_ui_colors { ui_color fill, frame; };
struct basic_ui_colors { ui_color font, back; };
struct check_ui_colors { ui_color mark, fill, frame; };
struct graph_ui_colors { ui_color line, area, grid, fill, frame; };
struct knob_ui_colors { ui_color fill, marker, drag, inner, outer, light, shadow; };
struct tab_header_ui_colors { ui_color header_font, font, back, inner_frame, active_font, active_back, outer_frame; };

// Represents all customizable colors.
struct part_ui_colors
{
  ui_color label, border, info_label, header_label, connector;
  knob_ui_colors knob;
  graph_ui_colors graph;
  basic_ui_colors edit, menu, knob_menu, table_menu;
  tab_header_ui_colors tab_header;
  check_ui_colors check, header_check;
  box_ui_colors param, param_container, header_container;
};

// Table mode parameters.
struct part_table_descriptor
{
  bool const ltr; // Left-to-right table display (e.g. headers are row headers). Column count should include the label column.
  std::int32_t const columns; // Number of columns for table display, or -1 for regular mode.
  float const* const widths; // Table display column factors (optional), should add up to 1.
  char const* const* const headers; // Headers for table display. When null and table_columns != -1, control labels are placed in each table cell.
};

// Special parameters.
struct special_params_descriptor
{
  std::int32_t const enabled; // Index into own params which identifies the part on/off switch, or -1 if always on.
  std::int32_t const enabled_relevant_if_rt_index_gt; // Enabled param has no effect if rt part index < N.
  std::int32_t const selector; // Index into selector params which identifies the part index switch param if this part type has more then 1 runtime part, or -1 if part count is 1.
};

// No selector, no per-part enabled switch.
inline special_params_descriptor constexpr part_no_special = { -1, 0, -1 };

struct part_ui_descriptor
{
  std::int32_t const columns; // Number of parameters stacked left-to-right. In table mode, only used to determine the total width.
  special_params_descriptor const special_params; // Special params.
  std::vector<graph_descriptor> const graphs; // Graphs.
  part_ui_colors const colors; // Color scheme.
  std::vector<box_descriptor> const borders; // Optional extra inner borders.
  part_table_descriptor const* const table; // Table mode params.
  std::int32_t const connectors; // Optional arrows to other parts.
  char const* const background_image; // Optional file name. Must be present in the resources folder.
  std::int32_t const background_voffset; // Background vertical offset.
  char const* const info; // Extra info for part header ui.
  bool const selector_space; // E.g. "Osc1" vs "LFO A1".
};

} // namespace inf::base
#endif // INF_BASE_TOPOLOGY_PART_UI_DESCRIPTOR_HPP