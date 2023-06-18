#include <inf.base/shared/io_stream.hpp>

#include <set>
#include <vector>
#include <cassert>
#include <iostream>
#include <algorithm>

namespace inf::base {

static std::string const magic = "{17026466-059D-4C8C-A13E-510250D72F46}";

static std::uint32_t
to_file_version(std::uint16_t major, std::uint16_t minor)
{ return major << 16 | minor; }

static void
from_file_version(std::uint32_t version, std::uint16_t& major, std::uint16_t& minor)
{
  minor = (version & 0x0000FFFF);
  major = (version & 0xFFFF0000) >> 16;
}

bool
io_stream::save(topology_info const& topology, param_value const* state)
{
  std::size_t chars;
  std::vector<char> str;
  
  assert(state != nullptr);
  topology.state_check(state);
  std::uint32_t file_version = to_file_version(topology.version_major(), topology.version_minor());

  if(!write_string(magic)) return false;
  if(!write_int32(file_version)) return false;
  if(!write_int32(topology.input_param_count)) return false;

  for (std::int32_t p = 0; p < topology.input_param_count; p++)
  {
    auto const& param = *topology.params[p].descriptor;
    auto const& part = topology.parts[topology.params[p].part_index];

    if(!write_string(part.descriptor->guid)) return false;
    if(!write_int32(part.type_index)) return false;
    if(!write_string(param.guid)) return false;
    if(!write_int32(param.data.io_type())) return false;

    switch (param.data.io_type())
    {
    case param_io::text:
      chars = param.data.format(true, state[p], nullptr, 0);
      str.reserve(chars);
      param.data.format(true, state[p], str.data(), chars);
      if (!write_string(std::string(str.data()))) return false;
      break;
    case param_io::real: if(!write_float(state[p].real)) return false; break;
    case param_io::discrete: if(!write_int32(state[p].discrete)) return false; break;
    default: assert(false); break;
    }
  }
  return true;
}

bool
io_stream::load(topology_info const& topology, param_value* state)
{
  param_value value;
  std::int32_t io_type;
  std::string str_value;
  std::string part_guid;
  std::string param_guid;

  std::uint16_t old_major;
  std::uint16_t old_minor;
  std::string file_magic;
  std::int32_t type_index;
  std::int32_t param_count;
  std::uint32_t raw_file_version;

  assert(state != nullptr);
  std::uint32_t raw_version = to_file_version(topology.version_major(), topology.version_minor());

  if(!read_string(file_magic) || file_magic != magic)
  {
    std::cout << "InfernalSynth: not a patch file." << std::endl;
    return false;
  }
  if(!read_uint32(raw_file_version) || raw_file_version > raw_version)
  {
    std::cout << "InfernalSynth: incompatible file format." << std::endl;
    return false;
  }
  if(!read_int32(param_count) || param_count <= 0) 
  {
    std::cout << "InfernalSynth: wrong parameter count." << std::endl;
    return false;
  }

  // Set defaults in case some params are missing or invalid.
  topology.init_all_param_defaults(state);
  from_file_version(raw_file_version, old_major, old_minor);

  std::set<std::int32_t> matched_parameter_indices;
  for (std::int32_t sp = 0; sp < param_count; sp++)
  {
    if(!read_string(part_guid))
    {
      std::cout << "InfernalSynth: failed to read part guid." << std::endl;
      return false;
    }
    if(!read_int32(type_index))
    {
      std::cout << "InfernalSynth: failed to read part index." << std::endl;
      return false;
    }
    if(!read_string(param_guid))
    {
      std::cout << "InfernalSynth: failed to read parameter guid." << std::endl;
      return false;
    }
    if(!read_int32(io_type))
    {
      std::cout << "InfernalSynth: failed to read parameter io type." << std::endl;
      return false;
    }
    if(io_type < 0 || io_type >= param_io::count)
    {
      std::cout << "InfernalSynth: invalid parameter io type." << std::endl;
      return false;
    }

    str_value.clear();
    value = param_value();

    switch (io_type)
    {
    case param_io::real: 
      if (!read_float(value.real))
      {
        std::cout << "InfernalSynth: failed to read real parameter value." << std::endl;
        return false;
      }
      break;
    case param_io::text:
      if(!read_string(str_value)) 
      {
        std::cout << "InfernalSynth: failed to read text parameter value." << std::endl;
        return false;
      }
      break;
    case param_io::discrete: 
      if(!read_int32(value.discrete)) 
      {
        std::cout << "InfernalSynth: failed to read int parameter value." << std::endl;
        return false;
      }
      break;
    default: assert(false); break;
    }

    for (std::int32_t rp = 0; rp < topology.input_param_count; rp++)
    {
      std::int32_t part_index = topology.params[rp].part_index;
      auto const& param = topology.params[rp].descriptor;
      auto const& part = topology.parts[part_index];
      if(part_guid != part.descriptor->guid) continue;
      if(type_index != part.type_index) continue;
      if(param_guid != param->guid) continue;
      if(io_type != param->data.io_type()) continue;

      // Matched this one.
      matched_parameter_indices.insert(rp);

      // Read old stuff.
      switch (io_type)
      { 
      case param_io::real: state[rp].real = value.real; break;
      case param_io::discrete: state[rp].discrete = value.discrete; break;
      case param_io::text: if (param->data.parse(true, part_index, str_value.data(), value)) state[rp] = value; break;
      default: assert(false); break;
      }

      // Convert to new stuff.
      state[rp] = topology.convert_param(rp, state[rp], str_value, old_major, old_minor);

      // Make sure we're in range.
      switch (param->data.type)
      { 
      case param_type::real: state[rp].real = std::clamp(state[rp].real, 0.0f, 1.0f); break;
      default: state[rp].discrete = std::clamp(
        state[rp].discrete, param->data.discrete.min, 
        param->data.discrete.effective_max(part_index)); break;
      }

      break;
    }
  }

  // Report not-found parameters.
  for (std::int32_t rp = 0; rp < topology.input_param_count; rp++)
    if (matched_parameter_indices.find(rp) == matched_parameter_indices.end())
    {
      auto const& desc = topology.params[rp].descriptor;
      std::cout << "Missing value for " << desc->guid << " " << "(" << desc->data.static_name.short_ << ")" << std::endl;
    }

  topology.state_check(state);
  return true;
}
  
} // namespace inf::base