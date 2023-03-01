#include <inf.vst.tool/diff/checker.hpp>
#include <inf.vst.tool/shared/load_topology.hpp>

#include <set>
#include <fstream>
#include <iostream>

using namespace inf::base;
using namespace inf::vst::tool::shared;

namespace inf::vst::tool::diff {

static std::int32_t
find_param(topology_info const* seek, topology_info const* find, std::int32_t seek_index)
{
  auto const& param_seek = *seek->params[seek_index].descriptor;
  auto const& part_seek = seek->parts[seek->params[seek_index].part_index];
  for (std::int32_t i = 0; i < find->params.size(); i++)
  {
    auto const& param_find = *find->params[i].descriptor;
    auto const& part_find = find->parts[find->params[i].part_index];
    if (part_seek.descriptor->guid == part_find.descriptor->guid &&
      part_seek.type_index == part_find.type_index &&
      param_seek.guid == param_find.guid)
        return i;
  }
  return -1;
}

std::int32_t
diff_plugin(char const* library1_path, char const* library2_path)
{
  std::vector<std::string> added_names;
  std::vector<std::string> removed_names;
  std::unique_ptr<loaded_topology> loaded_topo1 = load_topology(library1_path);
  std::unique_ptr<loaded_topology> loaded_topo2 = load_topology(library2_path);
  if(!loaded_topo1 || !loaded_topo2) return 1;

  topology_info const* topo1 = loaded_topo1->topology();
  topology_info const* topo2 = loaded_topo2->topology();
  
  for(std::int32_t i = 0; i < topo1->params.size(); i++)
    if(find_param(topo1, topo2, i) == -1)
      removed_names.push_back(topo1->params[i].runtime_name);
  for (std::int32_t i = 0; i < topo2->params.size(); i++)
    if (find_param(topo2, topo1, i) == -1)
      added_names.push_back(topo2->params[i].runtime_name);
  
  for(std::int32_t i = 0; i < removed_names.size(); i++)
    if(std::find(added_names.begin(), added_names.end(), removed_names[i]) == added_names.end())
      std::cout << "Removed " << removed_names[i] << ".\n";
    else
      std::cout << "Changed id: " << removed_names[i] << ".\n";
  for (std::int32_t i = 0; i < added_names.size(); i++)
    if (std::find(removed_names.begin(), removed_names.end(), added_names[i]) == removed_names.end())
      std::cout << "Added " << added_names[i] << ".\n";
    else
      std::cout << "Changed id: " << added_names[i] << ".\n";

  return 0;
}

} // namespace inf::vst::tool::diff