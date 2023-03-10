#include <inf.vst.base/sdk/parameter.hpp>
#include <inf.vst.base/shared/support.hpp>
#include <inf.vst.base/shared/io_stream.hpp>
#include <inf.vst.tool/diff/checker.hpp>
#include <inf.vst.tool/shared/load_topology.hpp>

#include <base/source/fstreamer.h>
#include <public.sdk/source/vst/vstpresetfile.h>
#include <public.sdk/source/common/memorystream.h>
#include <public.sdk/source/common/memorystream.cpp>

#include <set>
#include <locale>
#include <codecvt>
#include <string>
#include <fstream>
#include <iostream>
#include <filesystem>

using namespace Steinberg;
using namespace Steinberg::Vst;

using namespace inf::base;
using namespace inf::vst::base;
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

std::int32_t
find_list_item(
  topology_info const* seek, std::int32_t seek_part, std::int32_t seek_param,
  topology_info const* find, std::int32_t find_part, std::int32_t find_param,
  std::int32_t seek_item)
{
  for(std::int32_t i = 0; i < find->static_parts[find_part].params[find_param].data.discrete.items->size(); i++)
    if((*seek->static_parts[seek_part].params[seek_param].data.discrete.items)[seek_item].id == 
       (*find->static_parts[find_part].params[find_param].data.discrete.items)[i].id)
      return i;
  return -1;
}

static std::int32_t
find_param(
  topology_info const* seek, topology_info const* find, std::int32_t seek_index)
{
  auto const& seek_param = *seek->params[seek_index].descriptor;
  auto const& seek_part = seek->parts[seek->params[seek_index].part_index];
  for (std::int32_t i = 0; i < find->params.size(); i++)
  {
    auto const& find_param = *find->params[i].descriptor;
    auto const& find_part = find->parts[find->params[i].part_index];
    if(seek_part.descriptor->guid == find_part.descriptor->guid &&
      seek_part.type_index == find_part.type_index &&
      seek_param.guid == find_param.guid)
      return i;
  }
  return -1;
}

static bool 
find_preset(char const* seek, char const* find, std::string seek_path, std::string& find_path)
{
  std::string suffix = seek_path.substr(std::strlen(seek));
  for (auto const& preset : std::filesystem::recursive_directory_iterator(find))
    if (!preset.is_directory())
    {
      auto seek_this = std::string(find) + suffix;
      if(std::filesystem::exists(seek_this))
      {
        find_path = seek_this;
        return true;
      }
    }
  return false;
}

static bool
load_preset(topology_info const* topo, param_value* state, char const* path)
{
  std::ifstream file(path, std::ios::binary | std::ios::ate);
  if(file.bad()) return false;
  std::streamsize size = file.tellg();
  file.seekg(0, std::ios::beg);
  std::vector<char> buffer(size);
  if (!file.read(buffer.data(), size)) return false;
  MemoryStream memory(buffer.data(), buffer.size());
  PresetFile preset(&memory);
  if (!preset.readChunkList()) return false;
  if (!preset.seekToComponentState()) return false;
  IBStreamer streamer(&memory, kLittleEndian);
  vst_io_stream stream(&streamer);
  return stream.load(*topo, state);
}

namespace inf::vst::tool::diff {

std::int32_t
check_plugin(
  char const* library1_path, 
  char const* library2_path)
{
  std::unique_ptr<loaded_topology> loaded_topo1 = load_topology(library1_path);
  std::unique_ptr<loaded_topology> loaded_topo2 = load_topology(library2_path);
  if(!loaded_topo1 || !loaded_topo2) return 1;
  topology_info const* topo1 = loaded_topo1->topology();
  topology_info const* topo2 = loaded_topo2->topology();

  std::cout << "Comparing " << topo2->plugin_name() << " (" << topo2->version_major() << "." << topo2->version_minor() << ") "
    << "against " << topo1->plugin_name() << " (" << topo1->version_major() << "." << topo1->version_minor() << ").\n";

  for(std::int32_t i = 0; i < topo1->static_part_count; i++)
    if(find_part(topo1, topo2, i) == -1)
      std::cout << topo1->static_parts[i].static_name.detail << " removed.\n";
  for (std::int32_t i = 0; i < topo2->static_part_count; i++)
    if (find_part(topo2, topo1, i) == -1)
      std::cout << topo2->static_parts[i].static_name.detail << " added.\n";
  
  for (std::int32_t i = 0; i < topo2->static_part_count; i++)
  {
    bool printed_part_diff = false;
    auto print_part_diff = [&](std::int32_t part_index)
    {
      if (printed_part_diff) return;
      printed_part_diff = true;
      std::cout << topo2->static_parts[part_index].static_name.detail << " diff:\n";
    };

    std::int32_t old_part_index = find_part(topo2, topo1, i);
    if (old_part_index < 0) continue;
    
    part_descriptor const* part2 = &topo2->static_parts[i];
    part_descriptor const* part1 = &topo1->static_parts[old_part_index];
    if(part1->kind != part2->kind) print_part_diff(i), std::cout << "\tChanged kind.\n";
    if(part1->type != part2->type) print_part_diff(i), std::cout << "\tChanged type.\n";
    if(part1->param_count != part2->param_count) print_part_diff(i), std::cout << "\tChanged param count.\n";
    if(std::string(part1->static_name.detail) != part2->static_name.detail) print_part_diff(i), std::cout << "\tChanged long name.\n";
    if(std::string(part1->static_name.short_) != part2->static_name.short_) print_part_diff(i), std::cout << "\tChanged short name.\n";
    if(part1->part_count != part2->part_count) print_part_diff(i), std::cout << "\tChanged part count from " << part1->part_count << " to " << part2->part_count << ".\n";

    for(std::int32_t j = 0; j < topo1->static_parts[old_part_index].param_count; j++)
      if(find_param(topo1, old_part_index, topo2, i, j) == -1)
        print_part_diff(i), std::cout << "\t" << topo1->static_parts[old_part_index].params[j].data.static_name.detail << " removed.\n";
    for (std::int32_t j = 0; j < topo2->static_parts[i].param_count; j++)
      if (find_param(topo2, i, topo1, old_part_index, j) == -1)
        print_part_diff(i), std::cout << "\t" << topo2->static_parts[i].params[j].data.static_name.detail << " added.\n";

    bool printed_param_diff = false;
    auto print_param_diff = [&](std::int32_t part_index, std::int32_t param_index)
    {
      print_part_diff(part_index);
      if (printed_param_diff) return;
      printed_param_diff = true;
      std::cout << "\tParam diff:\n";
    };

    for (std::int32_t j = 0; j < topo2->static_parts[i].param_count; j++)
    {
      bool printed_list_diff = false;
      auto print_list_diff = [&](std::int32_t part_index, std::int32_t param_index, std::int32_t list_index)
      {
        print_part_diff(part_index);
        print_param_diff(part_index, param_index);
        if (printed_list_diff) return;
        printed_list_diff = true;
        std::cout << "\t\tList diff:\n";
      };

      std::int32_t old_param_index = find_param(topo2, i, topo1, old_part_index, j);
      if(old_param_index < 0) continue;
      param_descriptor_data const* param2 = &topo2->static_parts[i].params[j].data;
      param_descriptor_data const* param1 = &topo1->static_parts[old_part_index].params[old_param_index].data;
      if(param1->type != param2->type) print_param_diff(i, j), std::cout << "\t\t" << param2->static_name.detail << " changed type.\n";
      if(param1->kind != param2->kind) print_param_diff(i, j), std::cout << "\t\t" << param2->static_name.detail << " changed kind.\n";
      if(std::string(param1->unit) != param2->unit) print_param_diff(i, j), std::cout << "\t\t" << param2->static_name.detail << " changed unit.\n";
      if(std::string(param1->static_name.detail) != param2->static_name.detail) print_param_diff(i, j), std::cout << "\t\t" << param2->static_name.detail << " changed long name.\n";
      if(std::string(param1->static_name.short_) != param2->static_name.short_) print_param_diff(i, j), std::cout << "\t\t" << param2->static_name.detail << " changed short name.\n";

      if (param1->type == param_type::real && param2->type == param_type::real)
      {
        if(param1->real.default_ != param2->real.default_) print_param_diff(i, j), std::cout << "\t\t" << param2->static_name.detail << " changed real default.\n";
        if(param1->real.precision != param2->real.precision) print_param_diff(i, j), std::cout << "\t\t" << param2->static_name.detail << " changed real precision.\n";
        if(param1->real.dsp.min != param2->real.dsp.min) print_param_diff(i, j), std::cout << "\t\t" << param2->static_name.detail << " changed real dsp min.\n";
        if(param1->real.dsp.max != param2->real.dsp.max) print_param_diff(i, j), std::cout << "\t\t" << param2->static_name.detail << " changed real dsp max.\n";
        if(param1->real.dsp.slope != param2->real.dsp.slope) print_param_diff(i, j), std::cout << "\t\t" << param2->static_name.detail << " changed real dsp slope.\n";
        if(param1->real.dsp.linear_max != param2->real.dsp.linear_max) print_param_diff(i, j), std::cout << "\t\t" << param2->static_name.detail << " changed real dsp linear max.\n";
      }
      else if(param1->type != param_type::real && param2->type != param_type::real)
      {
        if(param1->discrete.min != param2->discrete.min) print_param_diff(i, j), std::cout << "\t\t" << param2->static_name.detail << " changed discrete min.\n";
        if(param1->discrete.max != param2->discrete.max) print_param_diff(i, j), std::cout << "\t\t" << param2->static_name.detail << " changed discrete max.\n";
        if(param1->discrete.default_ != param2->discrete.default_) print_param_diff(i, j), std::cout << "\t\t" << param2->static_name.detail << " changed discrete default.\n";
        if (param1->type == param_type::list && param2->type == param_type::list)
        {
          for (std::int32_t k = 0; k < topo1->static_parts[old_part_index].params[old_param_index].data.discrete.items->size(); k++)
            if (find_list_item(topo1, old_part_index, old_param_index, topo2, i, j, k) == -1)
              print_list_diff(i, j, k), std::cout << "\t\t\t" << (*topo1->static_parts[old_part_index].params[old_param_index].data.discrete.items)[k].name << " removed.\n";
          for (std::int32_t k = 0; k < topo2->static_parts[i].params[j].data.discrete.items->size(); k++)
            if (find_list_item(topo2, i, j, topo1, old_part_index, old_param_index, k) == -1)
              print_list_diff(i, j, k), std::cout << "\t\t\t" << (*topo2->static_parts[i].params[j].data.discrete.items)[k].name << " added.\n";
        }
      }
    }
  }

  return 0;
}

std::int32_t
check_preset_file(
  char const* library1_path, char const* preset1_path, 
  char const* library2_path, char const* preset2_path)
{
  assert(preset1_path);
  assert(preset2_path);
  assert(library1_path);
  assert(library2_path);

  std::unique_ptr<loaded_topology> loaded_topo1 = load_topology(library1_path);
  std::unique_ptr<loaded_topology> loaded_topo2 = load_topology(library2_path);
  if (!loaded_topo1 || !loaded_topo2) return 1;
  topology_info const* topo1 = loaded_topo1->topology();
  topology_info const* topo2 = loaded_topo2->topology();

  std::cout << "Comparing " << preset2_path << " (" << topo2->plugin_name() << " (" << topo2->version_major() << "." << topo2->version_minor() << ")) "
    << "against " << preset1_path << " (" << topo1->plugin_name() << " (" << topo1->version_major() << "." << topo1->version_minor() << ")).\n";

  std::vector<param_value> state1(topo1->input_param_count, param_value());
  std::vector<param_value> state2(topo2->input_param_count, param_value());
  if(!load_preset(topo1, state1.data(), preset1_path)) return std::cout << "Failed to load " << preset1_path << ".\n", 1;
  if(!load_preset(topo2, state2.data(), preset2_path)) return std::cout << "Failed to load " << preset2_path << ".\n", 1;

  for (std::int32_t i = 0; i < topo1->input_param_count; i++)
  {
    std::int32_t new_index = find_param(topo1, topo2, i);
    if (new_index >= 0) continue;
    std::string old_ui_state_val = topo1->params[i].descriptor->data.format(false, state1[i]);
    std::string old_ui_default_val = topo1->params[i].descriptor->data.format(false, topo1->params[i].descriptor->data.default_value());
    if(old_ui_state_val != old_ui_default_val)
      std::cout << "Removed non-default: " << topo1->params[i].runtime_name << ": " << old_ui_state_val << ".\n";
  }

  for (std::int32_t i = 0; i < topo2->input_param_count; i++)
  {
    std::int32_t old_index = find_param(topo2, topo1, i);
    if (old_index < 0) continue;

    String128 ui_val1;
    String128 ui_val2;
    ParamID tag2 = topo2->param_index_to_id[i];
    ParamID tag1 = topo1->param_index_to_id[old_index];
    part_info const* part2 = &topo2->parts[topo2->params[i].part_index];
    part_info const* part1 = &topo1->parts[topo1->params[old_index].part_index];
    vst_parameter vst_param2(tag2, part2, &topo2->params[i]);
    vst_parameter vst_param1(tag1, part1, &topo1->params[old_index]);
    vst_param2.toString(base_to_vst_normalized(topo2, i, state2[i]), ui_val2);
    vst_param1.toString(base_to_vst_normalized(topo1, old_index, state1[old_index]), ui_val1);
    std::u16string new_ui_state_val_16(ui_val2);
    std::u16string old_ui_state_val_16(ui_val1);
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convertor;
    std::string new_ui_state_val = convertor.to_bytes(new_ui_state_val_16);
    std::string old_ui_state_val = convertor.to_bytes(old_ui_state_val_16);
    if (old_ui_state_val != new_ui_state_val)
      std::cout << "Changed UI value: " << topo2->params[i].runtime_name << ": " << old_ui_state_val << " to " << new_ui_state_val << ".\n";
  }

  return 0;
}

std::int32_t
check_preset_folder(
  char const* library1_path, char const* folder1_path,
  char const* library2_path, char const* folder2_path)
{
  assert(folder1_path);
  assert(folder2_path);

  std::string find_path;
  for (auto const& preset1 : std::filesystem::recursive_directory_iterator(folder1_path))
    if(!preset1.is_directory() && !find_preset(folder1_path, folder2_path, preset1.path().string(), find_path))
      std::cout << "Removed " << preset1 << ".\n";
  for (auto const& preset2 : std::filesystem::recursive_directory_iterator(folder2_path))
    if (!preset2.is_directory() && !find_preset(folder2_path, folder1_path, preset2.path().string(), find_path))
      std::cout << "Added " << preset2 << ".\n";
  for (auto const& preset2 : std::filesystem::recursive_directory_iterator(folder2_path))
    if (!preset2.is_directory() && find_preset(folder2_path, folder1_path, preset2.path().string(), find_path))
      check_preset_file(library1_path, find_path.c_str(), library2_path, preset2.path().string().c_str());

  return 0;
}

} // namespace inf::vst::tool::diff