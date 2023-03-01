#include <inf.vst.tool/diff/plugin/checker.hpp>
#include <inf.vst.tool/shared/load_topology.hpp>

#include <set>
#include <string>
#include <fstream>
#include <iostream>

using namespace inf::base;
using namespace inf::vst::tool::shared;

static std::int32_t
find_part(
  topology_info const* seek, 
  topology_info const* find, 
  std::int32_t seek_part)
{
  for(std::int32_t i = 0; i < find->static_part_count; i++)
    if(seek->static_parts[seek_part].guid == find->static_parts[i].guid)
      return i;
  return -1;
}

static std::int32_t
find_param(
  topology_info const* seek, std::int32_t seek_part, 
  topology_info const* find, std::int32_t find_part, 
  std::int32_t seek_param)
{
  for(std::int32_t i = 0; i < find->static_parts[find_part].param_count; i++)
    if(seek->static_parts[seek_part].params[seek_param].guid == find->static_parts[find_part].params[i].guid)
      return i;
  return -1;
}

namespace inf::vst::tool::diff::plugin {

std::int32_t
check(char const* library1_path, char const* library2_path)
{
  std::unique_ptr<loaded_topology> loaded_topo1 = load_topology(library1_path);
  std::unique_ptr<loaded_topology> loaded_topo2 = load_topology(library2_path);
  if(!loaded_topo1 || !loaded_topo2) return 1;
  topology_info const* topo1 = loaded_topo1->topology();
  topology_info const* topo2 = loaded_topo2->topology();

  for(std::int32_t i = 0; i < topo1->static_part_count; i++)
    if(find_part(topo1, topo2, i) == -1)
      std::cout << topo1->static_parts[i].static_name.detail << " removed.\n";
  for (std::int32_t i = 0; i < topo2->static_part_count; i++)
    if (find_part(topo2, topo1, i) == -1)
      std::cout << topo2->static_parts[i].static_name.detail << " added.\n";
  
  for (std::int32_t i = 0; i < topo2->static_part_count; i++)
  {
    std::int32_t old_part_index = find_part(topo2, topo1, i);
    if (old_part_index < 0) continue;
    
    std::cout << topo2->static_parts[i].static_name.detail << " diff:\n";
    part_descriptor const* part2 = &topo2->static_parts[i];
    part_descriptor const* part1 = &topo1->static_parts[old_part_index];
    if(part1->kind != part2->kind) std::cout << "\tChanged kind.\n";
    if(part1->type != part2->type) std::cout << "\tChanged type.\n";
    if(part1->param_count != part2->param_count) "\tChanged param count.\n";
    if(std::string(part1->static_name.detail) != part2->static_name.detail) std::cout << "\tChanged long name.\n";
    if(std::string(part1->static_name.short_) != part2->static_name.short_) std::cout << "\tChanged short name.\n";
    if(part1->part_count != part2->part_count) std::cout << "\tChanged part count from " << part1->part_count << " to " << part2->part_count << ".\n";

    std::cout << "\tParam diff:\n";
    for(std::int32_t j = 0; j < topo1->static_parts[old_part_index].param_count; j++)
      if(find_param(topo1, old_part_index, topo2, i, j) == -1)
        std::cout << "\t\t" << topo1->static_parts[old_part_index].params[j].data.static_name.detail << " removed.\n";
    for (std::int32_t j = 0; j < topo2->static_parts[i].param_count; j++)
      if (find_param(topo2, i, topo1, old_part_index, j) == -1)
        std::cout << "\t\t" << topo2->static_parts[i].params[j].data.static_name.detail << " added.\n";

    for (std::int32_t j = 0; j < topo2->static_parts[i].param_count; j++)
    {
      std::int32_t old_param_index = find_param(topo2, i, topo1, old_part_index, j);
      if(old_param_index < 0) continue;
      param_descriptor_data const* param2 = &topo2->static_parts[i].params[j].data;
      param_descriptor_data const* param1 = &topo1->static_parts[old_part_index].params[old_param_index].data;
      if(param1->type != param2->type) std::cout << "\t\t" << param2->static_name.detail << " changed type.\n";
      if(param1->kind != param2->kind) std::cout << "\t\t" << param2->static_name.detail << " changed kind.\n";
      if(std::string(param1->unit) != param2->unit) std::cout << "\t\t" << param2->static_name.detail << " changed unit.\n";
      if(std::string(param1->static_name.detail) != param2->static_name.detail) std::cout << "\t\t" << param2->static_name.detail << " changed long name.\n";
      if(std::string(param1->static_name.short_) != param2->static_name.short_) std::cout << "\t\t" << param2->static_name.detail << " changed short name.\n";
    }
  }

  return 0;
}

} // namespace inf::vst::tool::diff::plugin