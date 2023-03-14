#include <inf.vst.tool/ui/generator/support.hpp>
#include <inf.vst.tool/ui/generator/generator.hpp>
#include <inf.vst.tool/shared/load_topology.hpp>
#include <inf.base/topology/part_descriptor.hpp>
#include <inf.base/topology/param_descriptor.hpp>
#include <inf.base/topology/param_ui_descriptor.hpp>

#include <vstgui/uidescription/rapidjson/include/rapidjson/rapidjson.h>
#include <vstgui/uidescription/rapidjson/include/rapidjson/prettywriter.h>
#include <vstgui/uidescription/rapidjson/include/rapidjson/ostreamwrapper.h>

#include <set>
#include <map>
#include <fstream>
#include <iostream>

using namespace rapidjson;
using namespace inf::base;
using namespace inf::vst::tool::shared;

namespace inf::vst::tool::ui {

static std::string
get_option_menu_class(param_info const& param)
{
  auto items = param.descriptor->data.discrete.items;
  if(items != nullptr)
    for(std::size_t i = 0; i < items->size(); i++)
      if(items->at(i).submenu_path.size() > 0)
        return "inf_nested_option_menu";
  return "inf_option_menu_fix";
}

static void 
get_table_mode_coords(
  part_ui_description const& part, std::int32_t column, 
  std::int32_t column_span, std::int32_t& left, std::int32_t& width)
{
  assert(column < part.columns);

  float start = 0.0f;
  float span_width = 0.0f;
  float total_width = part.width - static_cast<float>(margin);
  float const* widths = part.info->descriptor->ui->table->widths;

  float epsilon = 1e-5;
  (void)epsilon;
  float width_check = 0.0f;
  for(std::int32_t i = 0; i < part.info->descriptor->ui->table->columns; i++)
    width_check += part.info->descriptor->ui->table->widths[i];
  assert(std::abs(width_check -  1.0f) < epsilon);

  for(std::int32_t c = 0; c < column; c++) start += widths[c];
  for(std::int32_t c = column; c < column + column_span; c++) span_width += widths[c];
  left = static_cast<std::int32_t>(std::floor(start * total_width));
  
  // Correct for missed pixels due to rounding.
  std::int32_t next_left;
  std::int32_t next_width;
  if (column == part.columns - column_span)
  {
    width = static_cast<std::int32_t>(std::ceil(span_width * total_width) - padding_param_group);
    return;
  }
  get_table_mode_coords(part, column + column_span, 1, next_left, next_width);
  width = next_left - left - padding_param_group;
}

static void 
get_box_coords(
  part_ui_description const& part, box_descriptor const& box,
  std::int32_t& left, std::int32_t& top, std::int32_t& width, std::int32_t& height)
{
  width = box.column_span * param_total_width - padding_param_group;
  left = box.column * param_total_width + 3 * padding_param_group - margin;
  top = box.row * (param_row_height + margin) + 3 * padding_param_group - margin;
  height = box.row_span * param_row_height + (box.row_span - 1) * margin + 2 * padding_param_group;
  if (part.info->descriptor->ui->table != nullptr) get_table_mode_coords(part, box.column, box.column_span, left, width);
}

static Value
build_ui_control_tags(
  topology_info const& topology, Document::AllocatorType& allocator)
{
  Value result(kObjectType);
  for (std::size_t p = 0; p < topology.params.size(); p++)
  {
    auto const& param = topology.params[p];
    auto const& part = topology.parts[param.part_index];
    if (param.descriptor->data.kind == param_kind::fixed) continue;
    std::string vst3_tag = std::to_string(topology.param_index_to_id[p]);
    add_member(result, get_control_tag(part, param), vst3_tag, allocator);
  }
  return result;
}

static Value
build_ui_bitmaps(
  topology_info const& topology, Document::AllocatorType& allocator)
{
  Value result(kObjectType);
  std::set<std::string> seen;
  for(std::int32_t i = 0; i < topology.static_part_count; i++)
    if(topology.static_parts[i].ui != nullptr)
    {
      char const* file = topology.static_parts[i].ui->background_image;
      if(file == nullptr || seen.find(file) != seen.end()) continue;
      seen.insert(file);
      Value background_value(kObjectType);
      add_member(background_value, "path", file, allocator);
      add_member(result, file, background_value, allocator);
    }
  return result;
}

static Value
build_ui_colors(
  topology_info const& topology, Document::AllocatorType& allocator)
{
  Value result(kObjectType);
  std::map<std::string, std::string> color_map;

  auto add_color = [&](std::string const& name, ui_color const& col) {
    auto iter = color_map.find(name);
    assert(iter == color_map.end());
    std::string value = get_color_value(col);
    color_map[name] = value;
    add_member(result, name, value, allocator); };
  
  add_color("transparent", 0x00FFFFFF);
  for (std::int32_t i = 0; i < topology.static_part_count; i++)
  {
    auto const& colors = topology.static_parts[i].ui->colors;
    if (topology.static_parts[i].kind == part_kind::selector) continue;
    add_color("label", colors.label);
    add_color("border", colors.border);
    add_color("connector", colors.connector);
    add_color("info_label", colors.info_label);
    add_color("header_label", colors.header_label);
    add_color("menu_back", colors.menu.back);
    add_color("menu_font", colors.menu.font);
    add_color("edit_back", colors.edit.back);
    add_color("edit_font", colors.edit.font);
    add_color("graph_area", colors.graph.area);
    add_color("graph_fill", colors.graph.fill);
    add_color("graph_grid", colors.graph.grid);
    add_color("graph_line", colors.graph.line);
    add_color("graph_frame", colors.graph.frame);
    add_color("knob_drag", colors.knob.drag);
    add_color("knob_fill", colors.knob.fill);
    add_color("knob_inner", colors.knob.inner);
    add_color("knob_outer", colors.knob.outer);
    add_color("knob_light", colors.knob.light);
    add_color("knob_marker", colors.knob.marker);
    add_color("knob_shadow", colors.knob.shadow);
    add_color("param_fill", colors.param.fill);
    add_color("param_frame", colors.param.frame);
    add_color("check_fill", colors.check.fill);
    add_color("check_mark", colors.check.mark);
    add_color("check_frame", colors.check.frame);
    add_color("knob_menu_back", colors.knob_menu.back);
    add_color("knob_menu_font", colors.knob_menu.font);
    add_color("table_menu_back", colors.table_menu.back);
    add_color("table_menu_font", colors.table_menu.font);
    add_color("header_check_fill", colors.header_check.fill);
    add_color("header_check_mark", colors.header_check.mark);
    add_color("header_check_frame", colors.header_check.frame);
    add_color("param_container_fill", colors.param_container.fill);
    add_color("param_container_frame", colors.param_container.frame);
    add_color("header_container_fill", colors.header_container.fill);
    add_color("header_container_frame", colors.header_container.frame);
    add_color("tab_header_back", colors.tab_header.back);
    add_color("tab_header_font", colors.tab_header.font);
    add_color("tab_header_active_back", colors.tab_header.active_back);
    add_color("tab_header_active_font", colors.tab_header.active_font);
    add_color("tab_header_header_font", colors.tab_header.header_font);
    add_color("tab_header_inner_frame", colors.tab_header.inner_frame);
    add_color("tab_header_outer_frame", colors.tab_header.outer_frame);
  }
  return result;
}

static Value
build_ui_param_item_base(
  std::string const& control_class, std::int32_t left, 
  std::int32_t width, std::int32_t top_margin, Document::AllocatorType& allocator)
{
  Value result(kObjectType);
  add_attribute(result, "class", control_class, allocator);
  add_attribute(result, "size", size_to_string(width, param_row_height), allocator);
  add_attribute(result, "origin", size_to_string(left, top_margin), allocator);
  return result;
}

static Value
build_ui_param_label(
  part_type_ui_description const& type, std::string const& alignment, std::int32_t left,
  std::int32_t width, std::string const& title, Document::AllocatorType& allocator)
{
  Value result(build_ui_param_item_base("CTextLabel", left, width, 0, allocator));
  add_attribute(result, "transparent", "true", allocator);
  add_attribute(result, "font", "~ NormalFontSmall", allocator);
  add_attribute(result, "text-alignment", alignment, allocator);
  add_attribute(result, "font-color", get_color_name(type.colors.label), allocator);
  add_attribute(result, "title", title, allocator);
  return result;
}

static Value
build_ui_param_control_base(
  part_info const& part, param_info const& param,
  std::string const& control_class, std::int32_t left, std::int32_t width, 
  std::int32_t top_margin, Document::AllocatorType& allocator)
{
  Value result(build_ui_param_item_base(control_class, left, width, top_margin, allocator));
  std::string tag = get_control_tag(part, param);
  add_attribute(result, "control-tag", tag, allocator);
  std::string tooltip = param.runtime_name;
  std::string unit = std::string(" (") + param.descriptor->data.unit + ")";
  if (std::strlen(param.descriptor->data.unit) > 0) tooltip += unit;
  add_attribute(result, "tooltip", tooltip, allocator);
  return result;
}

static Value
build_ui_param_checkbox(
  part_info const& part, param_info const& param, 
  check_ui_colors const& colors, std::int32_t left, std::int32_t width, 
  std::int32_t top_margin, Document::AllocatorType& allocator)
{
  Value result(build_ui_param_control_base(part, param, "CCheckBox", left, width, top_margin, allocator));
  add_attribute(result, "title", "", allocator);
  add_attribute(result, "frame-width", "1", allocator);
  add_attribute(result, "draw-crossbox", "true", allocator);
  add_attribute(result, "round-rect-radius", std::to_string(margin), allocator);
  add_attribute(result, "boxfill-color", get_color_name(colors.fill), allocator);
  add_attribute(result, "boxframe-color", get_color_name(colors.frame), allocator);
  add_attribute(result, "checkmark-color", get_color_name(colors.mark), allocator);
  return result;
}

static Value
build_ui_param_knob(
  part_type_ui_description const& type, part_info const& part, 
  param_info const& param, Document::AllocatorType& allocator)
{
  bool discrete = param.descriptor->data.type != param_type::real; 
  Value result(build_ui_param_control_base(part, param, "inf_rotary_knob", padding_param_group, param_col1_width, 0, allocator));
  add_attribute(result, "angle-start", "30", allocator);
  add_attribute(result, "angle-range", "300", allocator); 
  add_attribute(result, "discrete", discrete? "true": "false", allocator);
  add_attribute(result, "fill-color", get_color_name(type.colors.knob.fill), allocator);
  add_attribute(result, "marker-color", get_color_name(type.colors.knob.marker), allocator);
  add_attribute(result, "drag-color", get_color_name(type.colors.knob.drag), allocator);
  add_attribute(result, "inner-color", get_color_name(type.colors.knob.inner), allocator);
  add_attribute(result, "outer-color", get_color_name(type.colors.knob.outer), allocator);
  add_attribute(result, "light-color", get_color_name(type.colors.knob.light), allocator);
  add_attribute(result, "shadow-color", get_color_name(type.colors.knob.shadow), allocator);
  add_attribute(result, "bipolar", !discrete && param.descriptor->data.real.display.min < 0.0f? "true": "false", allocator);
  return result;
}

static Value
build_ui_param_menu(
  part_info const& part, param_info const& param, std::string const& alignment,
  std::int32_t left, std::int32_t width, std::int32_t top_margin, 
  basic_ui_colors const& colors, Document::AllocatorType& allocator)
{
  std::int32_t max_items_per_column = 32;
  std::int32_t item_count = param.descriptor->data.discrete.max + 1;
  std::int32_t column_count = static_cast<std::int32_t>(std::ceil(item_count / static_cast<float>(max_items_per_column)));
  std::int32_t items_per_column = static_cast<std::int32_t>(std::ceil(item_count / static_cast<float>(column_count)));

  auto klass = get_option_menu_class(param);
  Value result(build_ui_param_control_base(part, param, klass, left, width, top_margin, allocator));
  add_attribute(result, "min-value", "0", allocator);
  add_attribute(result, "default-value", "0", allocator);
  add_attribute(result, "style-no-frame", "true", allocator);
  add_attribute(result, "style-round-rect", "true", allocator);
  add_attribute(result, "menu-popup-style", "true", allocator);
  add_attribute(result, "menu-check-style", "false", allocator); 
  add_attribute(result, "font", "~ NormalFontSmall", allocator);
  add_attribute(result, "text-alignment", alignment, allocator);
  add_attribute(result, "font-color", get_color_name(colors.font), allocator);
  add_attribute(result, "back-color", get_color_name(colors.back), allocator);
  add_attribute(result, "text-inset", size_to_string(margin, 0), allocator);
  add_attribute(result, "round-rect-radius", std::to_string(margin), allocator);
  add_attribute(result, "items-per-column", std::to_string(items_per_column), allocator);
  add_attribute(result, "max-value", std::to_string(param.descriptor->data.discrete.max), allocator);
  return result;
}

static Value
build_ui_param_edit(
  part_type_ui_description const& type, part_info const& part, 
  param_info const& param, std::int32_t left, std::int32_t width,
  std::string const& alignment, Document::AllocatorType& allocator)
{
  Value result(build_ui_param_control_base(part, param, "CTextEdit", left, width, 0, allocator));
  add_attribute(result, "style-no-frame", "true", allocator);
  add_attribute(result, "style-round-rect", "true", allocator);
  add_attribute(result, "text-alignment", alignment, allocator);
  add_attribute(result, "font", "~ NormalFontSmall", allocator);
  add_attribute(result, "text-inset", size_to_string(margin, 0), allocator);
  add_attribute(result, "round-rect-radius", std::to_string(margin), allocator);
  add_attribute(result, "back-color", get_color_name(type.colors.edit.back), allocator);
  add_attribute(result, "font-color", get_color_name(type.colors.edit.font), allocator);
  add_attribute(result, "value-precision", std::to_string(param.descriptor->data.real.precision), allocator);
  return result;
}

static Value
build_ui_single_param_border(
  part_type_ui_description const& type, part_ui_description const& part, 
  std::int32_t row, std::int32_t column, Document::AllocatorType& allocator)
{
  Value result(kObjectType);
  std::int32_t width = param_total_width - padding_param_group;
  std::int32_t height = param_row_height + margin - padding_param_group;
  std::int32_t left = column * param_total_width + padding_param_group * 3 - margin;
  std::int32_t top = row * (param_row_height + margin) + 3 * padding_param_group - margin;
  if (part.info->descriptor->ui->table != nullptr) get_table_mode_coords(part, column, 1, left, width);

  add_attribute(result, "class", "inf_view_container_fix", allocator);
  add_attribute(result, "origin", size_to_string(left, top), allocator);
  add_attribute(result, "size", size_to_string(width, height), allocator);
  add_attribute(result, "background-color-draw-style", "stroked", allocator);
  add_attribute(result, "background-color", get_color_name(type.colors.param.frame), allocator);
  return result;
}

static void
add_ui_input_param(
  part_type_ui_description const& type, part_ui_description const& part,
  param_ui_description const& param, Value& container, Document::AllocatorType& allocator)
{
  std::int32_t left_col2 = param_col1_width + margin;
  std::int32_t left_col3 = left_col2 + param_col2_width + margin;
  std::int32_t col12_and_margin = param_col1_width + margin + param_col2_width;
  
  std::int32_t label_left = left_col2;
  std::int32_t label_width = param_col2_width;
  std::int32_t menu_left = col12_and_margin;
  std::int32_t menu_width = param_col3_width;
  std::int32_t edit_left = col12_and_margin;
  std::int32_t edit_width = param_col3_width;

  part_ui_descriptor const* part_ui = part.info->descriptor->ui;
  if (part_ui->table != nullptr)
  {
    std::int32_t cell_left;
    std::int32_t cell_width;
    get_table_mode_coords(part, param.column, 1, cell_left, cell_width); 
    menu_left = 0;
    edit_left = param_col1_width;
    label_left += 2 * padding_param_group;
    menu_width = cell_width - 3 * padding_param_group;
    edit_width = cell_width - param_col1_width - 3 * padding_param_group;
  }

  param_descriptor_data const& param_desc = param.info->descriptor->data;
  if (param_desc.ui != nullptr && param_desc.ui->menu_wide)
  {
    assert(part.info->descriptor->ui->table == nullptr);
    label_left = 3 * padding_param_group;
    label_width = param_col1_width;
    menu_left = left_col2;
    menu_width = param_col2_width + param_col3_width;
  }

  std::string title = param_desc.static_name.short_;
  switch (param_desc.type)
  {
  case param_type::text:
    add_child(container, "CTextEdit", build_ui_param_edit(
      type, *part.info, *param.info, 0, col12_and_margin, "right", allocator), allocator);
    add_child(container, "CTextLabel", build_ui_param_label(
      type, "left", left_col3, param_col3_width, title, allocator), allocator);
    break;
  case param_type::list:
    add_child(container, get_option_menu_class(*param.info), build_ui_param_menu(
      *part.info, *param.info, "right", menu_left, menu_width, 0, 
      part_ui->table == nullptr? type.colors.menu: type.colors.table_menu, allocator), allocator);
    if(part_ui->table == nullptr) add_child(container, "CTextLabel", build_ui_param_label(
      type, "left", label_left, label_width, title, allocator), allocator);
    break;
  case param_type::real:
  case param_type::knob:
    add_child(container, "inf_rotary_knob", build_ui_param_knob(
      type, *part.info, *param.info, allocator), allocator);
    add_child(container, "CTextEdit", build_ui_param_edit(
       type, *part.info, *param.info, edit_left, edit_width, "right", allocator), allocator);
    if(part_ui->table == nullptr) add_child(container, "CTextLabel", build_ui_param_label(
      type, "left", left_col2, param_col2_width, title, allocator), allocator);
    break;
  case param_type::list_knob:
  case param_type::knob_list:
    assert(part_ui->table == nullptr);
    add_child(container, "inf_rotary_knob", build_ui_param_knob(
      type, *part.info, *param.info, allocator), allocator);
    add_child(container, get_option_menu_class(*param.info), build_ui_param_menu(
      *part.info, *param.info, "right", menu_left, menu_width, 0, type.colors.knob_menu, allocator), allocator);
    add_child(container, "CTextLabel", build_ui_param_label(
      type, "left", left_col2, param_col2_width, title, allocator), allocator);
    break;
  case param_type::toggle:
    add_child(container, "CCheckBox", build_ui_param_checkbox(
      *part.info, *param.info, type.colors.check, margin + 2 * padding_param_group, param_col1_width, 0, allocator), allocator);
    add_child(container, "CTextLabel", build_ui_param_label(
      type, "left", left_col2, param_col2_width + margin + param_col3_width, title, allocator), allocator);
    break;  
  default:
    assert(false);
  }
}

static void
add_ui_output_param(
  part_type_ui_description const& type, part_ui_description const& part,
  param_ui_description const& param, Value& container, Document::AllocatorType& allocator)
{
  param_descriptor_data const& param_desc = param.info->descriptor->data;
  std::string title = param_desc.static_name.short_;
  std::int32_t label_left = padding_param_group;
  std::int32_t label_width = param_output_col_width;
  std::int32_t edit_width = param_output_col_width - margin;
  std::int32_t edit_left = param_output_col_width + margin;
  std::int32_t check_width = param_output_col_width;
  std::int32_t check_left = param_output_col_width + margin + padding_param_group;

  part_ui_descriptor const* part_ui = part.info->descriptor->ui;
  if (part_ui->table != nullptr)
  {
    std::int32_t cell_left;
    std::int32_t cell_width;
    assert(part.info->descriptor->ui->table->headers == nullptr);
    get_table_mode_coords(part, param.column, 1, cell_left, cell_width);
    check_width = checkbox_width;
    label_left += 2 * padding_param_group;
    edit_left = cell_width / 2 + padding_param_group;
    check_left = cell_width - check_width - 2 * padding_param_group;
    edit_width = label_width = cell_width / 2 - 3 * padding_param_group;
    if (param_desc.ui != nullptr && param_desc.ui->label_width > 0.0f)
    {
      label_width = static_cast<std::int32_t>(cell_width * param_desc.ui->label_width);
      edit_left = label_left + label_width + margin;
      edit_width = static_cast<std::int32_t>(cell_width * (1.0f - param_desc.ui->label_width)) - 2 * margin - padding_param_group;
    }
  } 

  add_child(container, "CTextLabel", build_ui_param_label(
    type, "left", label_left, param_output_col_width, title, allocator), allocator);
  switch (param_desc.type)
  {
  case param_type::text: 
    add_child(container, "CTextEdit", build_ui_param_edit(
      type, *part.info, *param.info, edit_left, edit_width, "right", allocator), allocator);
    break;
  case param_type::toggle: 
    add_child(container, "CCheckBox", build_ui_param_checkbox(
      *part.info, *param.info, type.colors.check, check_left, check_width, 0, allocator), allocator);
    break;
  case param_type::list:
    add_child(container, get_option_menu_class(*param.info), build_ui_param_menu(
      *part.info, *param.info, "right", edit_left, edit_width, 0,
      part_ui->table == nullptr ? type.colors.menu : type.colors.table_menu, allocator), allocator);
    break;
  default: 
    assert(false);
    break;
  }
}

// Decorator cell when param and header are null, row/column header when header is non-null,
// regular parameter cell otherwise. Note: only decorator cells have background. Actual 
// content cells are transparent, it's easier this way to deal with conditional visibility.
static Value
build_ui_part_single_param_container(
  part_type_ui_description const& type, part_ui_description const& part,
  param_ui_description const* param, std::int32_t row, std::int32_t column, 
  char const* header, Document::AllocatorType& allocator)
{
  Value result(kObjectType);
  part_ui_descriptor const* part_ui = part.info->descriptor->ui;
  std::int32_t width = param_total_width - padding_param_group * 3;
  std::int32_t left = column * param_total_width + 4 * padding_param_group - margin;
  std::int32_t top = row * (param_row_height + margin) + 4 * padding_param_group - margin;
  if (part_ui->table != nullptr)
  {
    get_table_mode_coords(part, column, 1, left, width);
    left += padding_param_group;
    width -= 2 * padding_param_group;
  }

  add_attribute(result, "class", "inf_view_container_fix", allocator);
  add_attribute(result, "origin", size_to_string(left, top), allocator);
  add_attribute(result, "background-color-draw-style", "filled", allocator);
  add_attribute(result, "size", size_to_string(width, param_row_height), allocator);

  // Decorator cell.
  if(param == nullptr && header == nullptr)
  {
    add_attribute(result, "background-color", get_color_name(type.colors.param.fill), allocator);
    return result;
  }

  add_attribute(result, "background-color", get_color_name(transparent), allocator);
  std::string alignment = part_ui->table != nullptr && part_ui->table->ltr ? "left" : "right";
  if (header != nullptr)
    add_child(result, "CTextLabel", build_ui_param_label(
      type, alignment, 3 * padding_param_group, width - margin - 3 * padding_param_group, header, allocator), allocator);
  else if (part.info->descriptor->kind == part_kind::output) 
    add_ui_output_param(type, part, *param, result, allocator);
  else 
    add_ui_input_param(type, part, *param, result, allocator);
  return result;
}

static Value
build_ui_part_graph(
  part_type_ui_description const& type, part_ui_description const& part,
  graph_descriptor const& graph, Document::AllocatorType& allocator)
{
  Value result(kObjectType);
  std::int32_t left, top, width, height;
  get_box_coords(part, graph.box, left, top, width, height);
  add_attribute(result, "class", "inf_graph_plot", allocator);
  add_attribute(result, "tooltip", graph.description, allocator);
  add_attribute(result, "origin", size_to_string(left, top), allocator);
  add_attribute(result, "size", size_to_string(width, height), allocator);
  add_attribute(result, "graph-type", std::to_string(graph.type), allocator);
  add_attribute(result, "row-span", std::to_string(graph.box.row_span), allocator);
  add_attribute(result, "part-index", std::to_string(part.info->type_index), allocator);
  add_attribute(result, "column-span", std::to_string(graph.box.column_span), allocator);
  add_attribute(result, "line-color", get_color_name(type.colors.graph.line), allocator);
  add_attribute(result, "fill-color", get_color_name(type.colors.graph.fill), allocator);
  add_attribute(result, "grid-color", get_color_name(type.colors.graph.grid), allocator);
  add_attribute(result, "area-color", get_color_name(type.colors.graph.area), allocator);
  add_attribute(result, "frame-color", get_color_name(type.colors.graph.frame), allocator);
  add_attribute(result, "part-type", std::to_string(part.info->descriptor->type), allocator);
  return result;
} 

static Value
build_ui_part_param_container_inner_border(
  part_type_ui_description const& type, part_ui_description const& part,
  std::int32_t left, std::int32_t top, std::int32_t width, std::int32_t height, 
  Document::AllocatorType& allocator)
{
  Value result(kObjectType);
  add_attribute(result, "class", "CTextLabel", allocator);
  add_attribute(result, "origin", size_to_string(left, top), allocator);
  add_attribute(result, "size", size_to_string(width, height), allocator);
  add_attribute(result, "back-color", get_color_name(transparent), allocator);
  add_attribute(result, "frame-color", get_color_name(type.colors.border), allocator);
  return result;
}

static void
add_cell_decoration(
  part_type_ui_description const& type, part_ui_description const& part, 
  std::int32_t row, std::int32_t column, Value& container, Document::AllocatorType& allocator)
{
  add_child(container, "inf_view_container_fix", build_ui_single_param_border(
    type, part, row, column, allocator), allocator);
  add_child(container, "inf_view_container_fix", build_ui_part_single_param_container(
    type, part, nullptr, row, column, nullptr, allocator), allocator);
}

static Value
build_ui_part_param_container_base(part_type_ui_description const& type,
  part_ui_description const& part, Document::AllocatorType& allocator)
{
  Value result(kObjectType);
  std::int32_t top = margin + param_row_height + padding_param_group * 3;
  std::int32_t height = part.height - 2 * margin - param_row_height - padding_param_group * 2;
  add_attribute(result, "class", "inf_view_container_fix", allocator);
  add_attribute(result, "origin", size_to_string(margin - padding_param_group, top), allocator);
  add_attribute(result, "size", size_to_string(part.width - 2 * margin + 2 * padding_param_group, height), allocator);
  return result;
}

static Value
build_ui_part_param_container_image(part_type_ui_description const& type,
  part_ui_description const& part, Document::AllocatorType& allocator)
{
  Value result = build_ui_part_param_container_base(type, part, allocator);
  part_ui_descriptor const* part_ui = part.info->descriptor->ui;
  if (part_ui->background_image == nullptr) return result;
  std::int32_t offset = param_row_height + padding_param_group * 2;
  add_attribute(result, "bitmap", part_ui->background_image, allocator);
  add_attribute(result, "background-offset", size_to_string(0, part_ui->background_voffset + offset), allocator);
  return result;
}

static Value
build_ui_part_param_container(part_type_ui_description const& type,
  part_ui_description const& part, Document::AllocatorType& allocator)
{
  Value result = build_ui_part_param_container_base(type, part, allocator);
  part_ui_descriptor const* part_ui = part.info->descriptor->ui;
  std::int32_t top = margin + param_row_height + padding_param_group * 3;
  std::int32_t height = part.height - 2 * margin - param_row_height - padding_param_group * 2;
  add_attribute(result, "class", "inf_view_container_fix", allocator);
  add_attribute(result, "origin", size_to_string(margin - padding_param_group, top), allocator);
  add_attribute(result, "size", size_to_string(part.width - 2 * margin + 2 * padding_param_group, height), allocator);
  add_attribute(result, "background-color", get_color_name(type.colors.param_container.fill), allocator);

  // Build graph content.
  for (std::int32_t g = 0; g < part_ui->graphs.size(); g++)
    add_child(result, "inf_graph_plot", build_ui_part_graph(
      type, part, part_ui->graphs[g], allocator), allocator);

  // Build cell decoration.
  std::set<std::pair<std::int32_t, std::int32_t>> cells_decorated;
  for (std::size_t p = 0; p < part.params.size(); p++)
  {
    // Only add background and borders the first time we encounter this cell.
    auto const& param = part.params[p];
    if(cells_decorated.find({ param.row, param.column}) == cells_decorated.end())
      add_cell_decoration(type, part, param.row, param.column, result, allocator);
    cells_decorated.insert({ param.row, param.column });
  }

  // Build actual parameter content. 
  for (std::int32_t p = 0; p < part.params.size(); p++)
    if (part.params[p].info != nullptr)
      add_child(result, "inf_view_container_fix", build_ui_part_single_param_container(
        type, part, &part.params[p], part.params[p].row, part.params[p].column, nullptr, allocator), allocator);

  // Table mode row headers.
  if (part_ui->table != nullptr && part_ui->table->headers != nullptr && part_ui->table->ltr)
    for (std::int32_t i = 0; i < part.rows; i++)
    {
      add_cell_decoration(type, part, i, 0, result, allocator);
      add_child(result, "inf_view_container_fix", build_ui_part_single_param_container(
        type, part, nullptr, i, 0, part_ui->table->headers[i], allocator), allocator);
    }
    
  // Table mode column headers.
  if (part_ui->table != nullptr && part_ui->table->headers != nullptr && !part_ui->table->ltr)
    for (std::int32_t i = 0; i < part.columns; i++)
    {
      add_cell_decoration(type, part, 0, i, result, allocator);
      add_child(result, "inf_view_container_fix", build_ui_part_single_param_container(
        type, part, nullptr, 0, i, part_ui->table->headers[i], allocator), allocator);
    }

  // Build borders last so opaque borders do not blend with the background.
  // Since this overlays everything below it we must draw 4 individual
  // containers, 1 for each line/side of the box, because a single view
  // container with borders prevents clicking on everything below it.
  for (std::int32_t i = 0; i < part_ui->borders.size(); i++)
  {
    std::int32_t l, t, w, h;
    get_box_coords(part, part_ui->borders[i], l, t, w, h);
    add_child(result, "CTextLabel", build_ui_part_param_container_inner_border(
      type, part, l, t, 1, h, allocator), allocator);
    add_child(result, "CTextLabel", build_ui_part_param_container_inner_border(
      type, part, l + w - 1, t, 1, h, allocator), allocator);
    add_child(result, "CTextLabel", build_ui_part_param_container_inner_border(
      type, part, l, t, w, 1, allocator), allocator);
    add_child(result, "CTextLabel", build_ui_part_param_container_inner_border(
      type, part, l, t + h - 1, w, 1, allocator), allocator);
  }
   
  return result;
}

static Value
build_ui_part_param_container_border(
  part_type_ui_description const& type, part_ui_description const& part,
  Document::AllocatorType& allocator)
{
  Value result(kObjectType);
  add_attribute(result, "class", "CTextLabel", allocator);
  add_attribute(result, "style-round-rect", "true", allocator);
  add_attribute(result, "round-rect-radius", std::to_string(margin), allocator);
  add_attribute(result, "back-color", get_color_name(type.colors.param_container.fill), allocator);
  add_attribute(result, "frame-color", get_color_name(type.colors.param_container.frame), allocator);
  add_attribute(result, "origin", size_to_string(0, param_row_height + padding_param_group * 4), allocator);
  add_attribute(result, "size", size_to_string(part.width, part.height - param_row_height - padding_param_group * 4), allocator);
  return result;
}

static Value
build_ui_part_header_label(
  std::string const& alignment, std::string const& title, 
  std::int32_t left, std::int32_t width, ui_color const& color, Document::AllocatorType& allocator)
{
  Value result(kObjectType);
  add_attribute(result, "title", title, allocator);
  add_attribute(result, "class", "CTextLabel", allocator);
  add_attribute(result, "transparent", "true", allocator);
  add_attribute(result, "font", "~ NormalFontSmall", allocator);
  add_attribute(result, "text-alignment", alignment, allocator);
  add_attribute(result, "font-color", get_color_name(color), allocator);
  add_attribute(result, "origin", size_to_string(left, -2 * padding_param_group), allocator);
  add_attribute(result, "size", size_to_string(width, param_row_height + padding_param_group * 4), allocator);
  return result;
} 

static Value
build_ui_part_header_container_base(part_type_ui_description const& type,
  part_ui_description const& part, Document::AllocatorType& allocator)
{
  Value result(kObjectType);
  std::int32_t left = margin - padding_param_group;
  std::int32_t top = margin - padding_param_group;
  std::int32_t width = part.width - 2 * margin + padding_param_group;
  std::int32_t height = param_row_height - margin + padding_param_group * 3;
  add_attribute(result, "class", "inf_view_container_fix", allocator);
  add_attribute(result, "origin", size_to_string(left, top), allocator);
  add_attribute(result, "size", size_to_string(width, height), allocator);
  return result;
}

static Value
build_ui_part_header_container_image(part_type_ui_description const& type,
  part_ui_description const& part, Document::AllocatorType& allocator)
{
  Value result = build_ui_part_header_container_base(type, part, allocator);
  part_ui_descriptor const* part_ui = part.info->descriptor->ui;
  if (part_ui->background_image == nullptr) return result;
  add_attribute(result, "bitmap", part_ui->background_image, allocator);
  add_attribute(result, "background-offset", size_to_string(0, part_ui->background_voffset), allocator);
  return result;
}

static Value
build_ui_part_connector(
  std::int32_t columns, connector_direction direction, 
  ui_color const& color, Document::AllocatorType& allocator)
{
  std::int32_t top = 0;
  std::int32_t left = -1;
  Value result(kObjectType); 

  switch (direction)
  {
  case connector_direction::up:
  case connector_direction::down: 
    left = (param_total_width * columns - header_connector_size) / 2;
    break;
  case connector_direction::left: 
  case connector_direction::right:
  case connector_direction::left_right:
  case connector_direction::halfway_left_up:
  case connector_direction::halfway_right_down:
    left = param_total_width * columns - header_connector_size - padding_param_group;
    break;
  default:
    assert(false); 
    break;
  }

  add_attribute(result, "class", "inf_part_connector", allocator);
  add_attribute(result, "origin", size_to_string(left, top), allocator);
  add_attribute(result, "connector-color", get_color_name(color), allocator);
  add_attribute(result, "connector-direction", std::to_string(direction), allocator);
  add_attribute(result, "size", size_to_string(header_connector_size, header_connector_size), allocator);
  return result;
}

static Value
build_ui_part_header_container(part_type_ui_description const& type, 
  part_ui_description const& part, Document::AllocatorType& allocator)
{
  Value result = build_ui_part_header_container_base(type, part, allocator);
  add_attribute(result, "background-color-draw-style", "filled", allocator);
  add_attribute(result, "background-color", get_color_name(type.colors.header_container.fill), allocator);

  std::int32_t enabled_width = 15;
  std::int32_t connectors_width = 0;
  auto connectors = part.info->descriptor->ui->connectors;
  if ((connectors & connector_direction::right) != 0 
    || (connectors & connector_direction::left) != 0
    || (connectors & connector_direction::halfway_left_up) != 0
    || (connectors & connector_direction::halfway_right_down) != 0)
      connectors_width = header_connector_size / 4 + margin + 2 * padding_param_group;
  if ((connectors & connector_direction::left_right) != 0) 
    connectors_width = header_connector_size / 2 + 2 * margin + 2 * padding_param_group;
  
  std::string title = " " + part.info->runtime_name;
  std::int32_t right = part.info->descriptor->ui->columns * param_total_width - margin; 
  std::int32_t check_left = right - checkbox_width - enabled_width - 2 * padding_param_group - connectors_width;
  if (part.enabled_param.info != nullptr && part.enabled_param_relevant)
  {  
    assert(type.selector_param.info != nullptr);
    Value enabled_box = build_ui_param_checkbox(*part.info, *part.enabled_param.info, 
      type.colors.header_check, check_left, checkbox_width, 0, allocator);
    add_child(result, "CCheckBox", enabled_box, allocator);
    std::string enabled = type.selector_param.info == nullptr? title: "On";
    add_child(result, "CTextLabel", build_ui_part_header_label("left", enabled, 
      right - enabled_width - connectors_width, enabled_width, type.colors.header_label, allocator), allocator);
  } else if(part.enabled_param.info == nullptr)
  {
    assert(type.selector_param.info == nullptr);
    add_child(result, "CTextLabel", build_ui_part_header_label("left", title, 
      2 * padding_param_group - margin, param_total_width, type.colors.header_label, allocator), allocator);
  } 

  auto const& ui = *part.info->descriptor->ui;
  std::int32_t info_left = (part.info->descriptor->ui->columns - 1) * param_total_width - connectors_width;
  std::int32_t info_width = param_total_width - margin - padding_param_group;
  if(part.enabled_param.info != nullptr) info_width -= checkbox_width + enabled_width + margin + 2 * padding_param_group;
  std::string info = std::string(" ") + ui.info;
  add_child(result, "CTextLabel", build_ui_part_header_label("right", 
    info, info_left, info_width, type.colors.info_label, allocator), allocator);

  for(std::int32_t connector = connector_direction::first; connector <= connector_direction::last; connector *= 2)
    if((ui.connectors & connector) != 0)
      add_child(result, "inf_part_connector", build_ui_part_connector(
        ui.columns, static_cast<connector_direction>(connector), ui.colors.connector, allocator), allocator);
  return result;
}

static Value
build_ui_part_header_container_border(part_type_ui_description const& type,
  part_ui_description const& part, Document::AllocatorType& allocator)
{
  Value result(kObjectType);
  add_attribute(result, "class", "CTextLabel", allocator);
  add_attribute(result, "style-round-rect", "true", allocator);
  add_attribute(result, "origin", size_to_string(0, 0), allocator);
  add_attribute(result, "round-rect-radius", std::to_string(margin), allocator);
  add_attribute(result, "back-color", get_color_name(type.colors.header_container.fill), allocator);
  add_attribute(result, "frame-color", get_color_name(type.colors.header_container.frame), allocator);
  add_attribute(result, "size", size_to_string(part.width, param_row_height + padding_param_group * 4), allocator);
  return result;
}

static Value
build_ui_part_container(part_type_ui_description const& type,
  part_ui_description const& part, Document::AllocatorType& allocator)
{
  Value result(kObjectType);
  add_attribute(result, "class", "inf_view_container_fix", allocator);
  add_attribute(result, "origin", size_to_string(0, 0), allocator);
  add_attribute(result, "background-color-draw-style", "stroked", allocator);
  add_attribute(result, "name", std::to_string(part.runtime_index), allocator);
  add_attribute(result, "size", size_to_string(part.width, part.height), allocator);
  add_attribute(result, "background-color", get_color_name(transparent), allocator);
  add_child(result, "CTextLabel", build_ui_part_header_container_border(type, part, allocator), allocator);
  add_child(result, "inf_view_container_fix", build_ui_part_header_container_image(type, part, allocator), allocator);
  add_child(result, "inf_view_container_fix", build_ui_part_header_container(type, part, allocator), allocator);
  add_child(result, "CTextLabel", build_ui_part_param_container_border(type, part, allocator), allocator);
  add_child(result, "inf_view_container_fix", build_ui_part_param_container_image(type, part, allocator), allocator);
  add_child(result, "inf_view_container_fix", build_ui_part_param_container(type, part, allocator), allocator);
  return result;
}

static Value
build_ui_part_switch_container(
  topology_info const& topology, part_type_ui_description const& type,
  Value& templates_part, Document::AllocatorType& allocator)
{
  Value result(kObjectType);
  std::string template_names = "";
  std::vector<std::string> template_names_list;
  part_info const& part = topology.parts[type.selector_param.info->part_index];
  std::string tag = get_control_tag(part, *type.selector_param.info);
  for (std::int32_t i = 0; i < type.parts.size(); i++)
  {
    std::string template_name = std::to_string(type.parts[i].runtime_index);
    template_names_list.push_back(template_name);
    template_names += template_name;
    if (i < type.parts.size() - 1) template_names += ",";
  }
  add_attribute(result, "animation-time", "0", allocator);
  add_attribute(result, "template-switch-control", tag, allocator);
  add_attribute(result, "origin", size_to_string(0, 0), allocator);
  add_attribute(result, "template-names", template_names, allocator);
  add_attribute(result, "class", "inf_view_switch_container_fix", allocator);
  add_attribute(result, "size", size_to_string(type.width, type.height), allocator);
  for (std::size_t i = 0; i < type.parts.size(); i++)
    add_member(templates_part, template_names_list[i],
      build_ui_part_container(type, type.parts[i], allocator), allocator);
  return result;
}

Value
build_ui_part_tab_header(part_type_ui_description const& type, 
  part_info const& part, Document::AllocatorType& allocator)
{
  std::int32_t top = padding_param_group;
  std::int32_t left = padding_param_group;
  std::int32_t width = 3 * param_total_width + padding_param_group;
  auto const& colors = type.colors.tab_header;
  auto const& selector_data = type.selector_param.info->descriptor->data;
  std::string items = selector_data.discrete.format_tab(selector_data.discrete.min);
  for(std::int32_t i = selector_data.discrete.min + 1; i <= selector_data.discrete.max; i++) 
    items += std::string(",") + selector_data.discrete.format_tab(i);

  Value result = build_ui_param_control_base(part,  *type.selector_param.info, "inf_tab_header", left, width, top, allocator);
  result.RemoveMember("size");
  add_attribute(result, "tab-items", items, allocator);
  add_attribute(result, "font", "~ NormalFontSmall", allocator); 
  add_attribute(result, "back-color", get_color_name(colors.back), allocator); 
  add_attribute(result, "font-color", get_color_name(colors.font), allocator);
  add_attribute(result, "header-font-color", get_color_name(colors.header_font), allocator);
  add_attribute(result, "inner-frame-color", get_color_name(colors.inner_frame), allocator);
  add_attribute(result, "outer-frame-color", get_color_name(colors.outer_frame), allocator);
  add_attribute(result, "active-back-color", get_color_name(colors.active_back), allocator);
  add_attribute(result, "active-font-color", get_color_name(colors.active_font), allocator);
  add_attribute(result, "tab-title", type.parts[0].info->descriptor->static_name.short_, allocator);
  add_attribute(result, "size", size_to_string(width, param_row_height + padding_param_group), allocator);
  return result;
}

static Value
build_ui_part_type_container(
  topology_info const& topology, part_type_ui_description const& type,
  Value& templates_part, Document::AllocatorType& allocator)
{
  Value result(kObjectType);
  add_attribute(result, "class", "inf_view_container_fix", allocator);
  add_attribute(result, "origin", size_to_string(type.left, type.top), allocator);
  add_attribute(result, "size", size_to_string(type.width, type.height), allocator);
  if (type.selector_param.info == nullptr)
    for (std::size_t i = 0; i < type.parts.size(); i++)
      add_child(result, "inf_view_container_fix", build_ui_part_container(
        type, type.parts[i], allocator), allocator);
  else
  {
    add_child(result, "inf_view_switch_container_fix", build_ui_part_switch_container(
      topology, type, templates_part, allocator), allocator);
    part_info const& part = topology.parts[type.selector_param.info->part_index];
    add_child(result, "inf_tab_header", build_ui_part_tab_header(type, part, allocator), allocator);
  }
  return result;
}

static Value
build_ui_template(
  topology_info const& topology, controller_ui_description const& controller,
  Value& templates_part, Document::AllocatorType& allocator)
{
  Value result(kObjectType);
  std::string size = size_to_string(controller.width, controller.height);
  add_attribute(result, "size", size, allocator);
  add_attribute(result, "minSize", size, allocator);
  add_attribute(result, "maxSize", size, allocator);
  add_attribute(result, "class", "inf_view_container_fix", allocator);
  for (std::size_t type = 0; type < controller.part_types.size(); type++)
    add_child(result, "inf_view_container_fix", build_ui_part_type_container(
      topology, controller.part_types[type], templates_part, allocator), allocator);
  return result;
}

static Document
build_vstgui_json(topology_info const& topology,
  controller_ui_description const& controller)
{
  Document result;
  result.SetObject();
  Document::AllocatorType& allocator = result.GetAllocator();
  Value root(kObjectType);
  Value templates(kObjectType);
  add_member(templates, "view", build_ui_template(topology, controller, templates, allocator), allocator);
  root.AddMember("version", "1", allocator);
  root.AddMember("bitmaps", build_ui_bitmaps(topology, allocator), allocator);
  root.AddMember("colors", build_ui_colors(topology, allocator), allocator);
  root.AddMember("control-tags", build_ui_control_tags(topology, allocator), allocator);
  root.AddMember("templates", templates, allocator);
  result.AddMember("vstgui-ui-description", root, allocator);
  return result;
}

static void
print_uidesc(topology_info const& topology, controller_ui_description const& desc)
{
  for (std::int32_t i = 0; i < desc.part_types.size(); i++)
  {
    auto const& type = desc.part_types[i];
    auto const& part = *type.parts[0].info->descriptor;
    std::cout << part.static_name.short_ << ": ";
    std::cout << part.part_count << " parts";
    std::cout << ", " << part.param_count << " params";
    std::cout << ", " << part.ui->graphs.size() << " graphs";
    if (type.selector_param.info != nullptr) std::cout << ", selector";
    if (type.parts[0].enabled_param.info != nullptr) std::cout << ", enabled";
    std::cout << "\n";
  }
  std::cout << "Topology: ";
  std::cout << topology.static_part_count << " static parts (including selector)";
  std::cout << ", " << topology.parts.size() << " runtime parts";
  std::cout << ", " << topology.params.size() << " runtime params";
  std::cout << "\n";
}

std::int32_t
write_vstgui_json(char const* library_path, char const* output_path)
{
  assert(output_path);
  assert(library_path);
  std::unique_ptr<loaded_topology> topology;
  if (!(topology = load_topology(library_path))) return 1;

  try
  {
    controller_ui_description description = controller_ui_description::create(*topology->topology());
    Document json(build_vstgui_json(*topology->topology(), description));
    std::ofstream os(output_path);
    OStreamWrapper wrapper(os);
    PrettyWriter<OStreamWrapper> writer(wrapper);
    json.Accept(writer);
    if (os.bad()) return 1;
    os.flush();
    os.close();
    print_uidesc(*topology->topology(), description);
  }
  catch (std::exception const& e)
  {
    std::cout << e.what() << "\n";
    return 1;
  }

  std::cout << "Ui description written to " << output_path << ".\n\n";
  return 0;
}

} // namespace inf::vst::tool::ui