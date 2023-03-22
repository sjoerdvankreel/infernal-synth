#include <inf.base/topology/part_descriptor.hpp>
#include <inf.tool.vst/ui/description/support.hpp>
#include <inf.tool.vst/ui/description/description.hpp>

#include <map>
#include <stdexcept>
#include <algorithm>

using namespace inf::base;

namespace inf::tool::vst::ui {

static bool 
param_inside_graph(
  part_ui_description const& part, 
  std::int32_t graph_index, std::int32_t grid_index)
{
  std::int32_t row = grid_index / part.columns;
  std::int32_t column = grid_index % part.columns;
  auto const& graph = part.info->descriptor->ui->graphs[graph_index];
  if(!(graph.box.row <= row && row < graph.box.row + graph.box.row_span)) return false;
  if(!(graph.box.column <= column && column < graph.box.column + graph.box.column_span)) return false;
  return true;
}

static bool 
param_inside_graphs(
  part_ui_description const& part, std::int32_t grid_index)
{
  for(std::int32_t g = 0; g < part.info->descriptor->ui->graphs.size(); g++)
    if(param_inside_graph(part, g, grid_index)) return true;
  return false;
}

part_ui_description 
part_ui_description::create(
  topology_info const& topology, part_info const* info, 
  std::int32_t static_part_index, std::int32_t runtime_index)
{
  part_ui_description result = {};
  part_descriptor const* desc = info->descriptor;
  part_ui_descriptor const* ui = desc->ui;

  result.info = info;
  result.occupied_cell_count = 0;
  result.runtime_index = runtime_index;
  for(std::int32_t i = 0; i < desc->param_count; i++)
    result.occupied_cell_count = std::max(result.occupied_cell_count, desc->params[i].data.ui_index + 1);
  for(std::int32_t g = 0; g < ui->graphs.size(); g++)
    result.occupied_cell_count += ui->graphs[g].box.row_span * ui->graphs[g].box.column_span;

  // param_columns determines width, in grid mode we fit grid cells to this width.
  result.columns = ui->table != nullptr ? ui->table->columns : ui->columns;
  std::int32_t non_header_columns = result.columns;
  if(ui->table != nullptr && ui->table->ltr) non_header_columns--;
  result.rows = result.occupied_cell_count / non_header_columns;

  // Column headers for top headered table mode.
  // Row headered table mode must already be included in the column count.
  if(ui->table != nullptr && ui->table->headers != nullptr && !ui->table->ltr) result.rows++;
  if (result.occupied_cell_count % non_header_columns != 0) result.rows++;
  result.width = ui->columns * param_total_width + margin;
  result.height = (result.rows + 1) * (param_row_height + margin) + padding_param_group * 4;

  result.enabled_param.row = 0;
  result.enabled_param.info = nullptr;
  result.enabled_param.column = result.columns - 1;
  result.enabled_param_relevant = static_part_index >= info->descriptor->ui->special_params.enabled_relevant_if_rt_index_gt;

  // Map ui index to parameter list (param id and skip-before count).
  std::map<std::int32_t, std::vector<std::int32_t>> layout;
  for (std::int32_t i = 0; i < desc->param_count; i++)
  {
    auto const& param_data = desc->params[i].data;
    if (param_data.kind == param_kind::fixed) continue;
    if (ui->special_params.enabled == i)
    {
      result.enabled_param.info = &topology.params[info->runtime_param_start + i];
      continue;
    }
    if (layout.find(param_data.ui_index) == layout.end())
      layout.insert(std::make_pair(param_data.ui_index, std::vector<std::int32_t>()));
    layout.at(param_data.ui_index).push_back(i);
  }
   
  // Build param positions around the (optional) graphs.
  std::int32_t grid_index = 0;
  std::int32_t param_columns = result.columns;
  if (ui->table != nullptr && ui->table->ltr) param_columns--;
  for(auto ui_index = layout.begin(); ui_index != layout.end(); ui_index++)
  {
    while (param_inside_graphs(result, grid_index)) grid_index++;
    for (auto param_index = ui_index->second.begin(); param_index != ui_index->second.end(); param_index++)
    {
      std::int32_t row = grid_index / param_columns;
      std::int32_t column = grid_index % param_columns;
      if (ui->table != nullptr && ui->table->ltr) column++; // Make room for row headers.
      if(ui->table != nullptr && ui->table->headers != nullptr && !ui->table->ltr) row++; // Make room for column headers.
      result.params.push_back({ row, column, &topology.params[info->runtime_param_start + *param_index] });
      assert(column < result.columns);
    }
    grid_index++;
  }

  // Add filler cells.
  if(ui->table == nullptr || ui->table->headers == nullptr)
    for (; grid_index < result.rows * result.columns; grid_index++)
    {
      while (param_inside_graphs(result, grid_index)) grid_index++;
      if(grid_index >= result.rows * result.columns) break;
      std::int32_t row = grid_index / result.columns;
      std::int32_t column = grid_index % result.columns;
      result.params.push_back({ row, column, nullptr });
    }

  return result;
}

// Make type descriptor with selector info.
part_type_ui_description
part_type_ui_description::create(
  topology_info const& topology, part_descriptor const& part,
  std::int32_t runtime_selector_indices_start, std::vector<std::int32_t> runtime_part_indices)
{
  part_type_ui_description result;
  result.selector_param.row = -1;
  result.selector_param.column = -1;
  result.selector_param.info = nullptr;
  result.colors = part.ui->colors;
  result.part_name = part.static_name.short_;
  for(std::int32_t i = 0; i < part.part_count; i++)
    result.parts.push_back(part_ui_description::create(topology,
      &topology.parts[runtime_part_indices[i]], i, runtime_part_indices[i]));
  if (part.ui->special_params.selector != -1)
    result.selector_param.info = &topology.params[runtime_selector_indices_start + part.ui->special_params.selector];
  result.width = result.parts[0].width;
  result.height = result.parts[0].height;
  return result;
}

controller_ui_description
controller_ui_description::create(topology_info const& topology)
{
  // For each part type, find out if it is the selector, otherwise store it's UI index.
  std::int32_t selector_part = -1;
  std::vector<std::pair<std::int32_t, std::int32_t>> part_type_ui_indices;
  for(std::int32_t i = 0; i < topology.static_part_count; i++)
    if(topology.static_parts[i].kind == part_kind::selector) selector_part = i;
    else part_type_ui_indices.push_back(std::make_pair(i, topology.static_parts[i].ui_index));

  // Sort part indices by ui index.
  std::sort(part_type_ui_indices.begin(), part_type_ui_indices.end(),
    [](auto const& l, auto const& r) -> bool { return l.second < r.second; });

  // Build type descriptions.
  controller_ui_description result;
  std::int32_t runtime_selector_indices_start = -1;
  if(selector_part != -1) runtime_selector_indices_start = topology.param_bounds[selector_part][0];
  for (std::size_t p = 0; p < part_type_ui_indices.size(); p++)
  {
    std::int32_t index = part_type_ui_indices[p].first;
    auto const& static_part = topology.static_parts[index];
    result.part_types.push_back(part_type_ui_description::create(
      topology, static_part, runtime_selector_indices_start, topology.part_bounds[index]));
  }

  // Fix up top/left/column.
  std::int32_t column = 0;
  std::int32_t top = margin;
  std::int32_t left = margin;
  std::int32_t column_width = 0;
  std::int32_t max_column_height = 0;
  for (std::size_t p = 0; p < result.part_types.size(); p++)
  {
    auto& part_type = result.part_types[p];
    if (top + part_type.height + margin > topology.max_ui_height)
    {
      // We went over the max height, move to next column.
      max_column_height = std::max(top, max_column_height);
      column++;
      top = margin;
      left += column_width + margin;
      result.column_widths.push_back(column_width);
      column_width = 0;
    }

    // Make cell for part type.
    part_type.top = top;
    part_type.left = left;
    part_type.column = column;
    column_width = std::max(column_width, part_type.width);
    top += part_type.height + margin;
  }
  result.column_widths.push_back(column_width);
  max_column_height = std::max(top, max_column_height);

  // Fix up column width in case of not-equal part ui widths.
  result.width = margin;
  for (std::size_t c = 0; c < result.column_widths.size(); c++)
    result.width += result.column_widths[c] + margin;
  for (std::size_t pt = 0; pt < result.part_types.size(); pt++)
  {
    result.part_types[pt].width = result.column_widths[result.part_types[pt].column];
    for (std::size_t p = 0; p < result.part_types[pt].parts.size(); p++)
      result.part_types[pt].parts[p].width = result.part_types[pt].width;
  }

  // Fix up last block height if we have spare room.
  result.height = max_column_height;
  for (std::size_t pt = 0; pt < result.part_types.size(); pt++)
    if (pt == result.part_types.size() - 1 || result.part_types[pt].column != result.part_types[pt + 1].column)
    {
      result.part_types[pt].height = result.height - result.part_types[pt].top - margin;
      for(std::size_t p = 0; p < result.part_types[pt].parts.size(); p++)
        result.part_types[pt].parts[p].height = result.part_types[pt].height;
    }

  return result;
}

} // namespace inf::tool::vst::ui