#include <inf.base/shared/io_stream.hpp>

#include <set>
#include <map>
#include <vector>
#include <cassert>
#include <iostream>
#include <algorithm>

namespace inf::base {

struct stored_param_id
{
  std::int32_t io_type;
  std::string part_guid;
  std::string param_guid;
  std::int32_t part_index;
};

bool operator<(stored_param_id const& l, stored_param_id const& r)
{ 
  if(l.io_type < r.io_type) return true;
  if(l.io_type > r.io_type) return false;
  if(l.part_index < r.part_index) return true;
  if(l.part_index > r.part_index) return false;
  if(l.part_guid < r.part_guid) return true;
  if(l.part_guid > r.part_guid) return false;
  if(l.param_guid < r.param_guid) return true;
  if(l.param_guid > r.param_guid) return false;
  return false;
}

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

  if(!read_string(file_magic) || file_magic != magic) return false;
  if(!read_uint32(raw_file_version) || raw_file_version > raw_version) return false;
  if(!read_int32(param_count) || param_count <= 0) return false;

  // Set defaults in case some params are missing or invalid.
  topology.init_all_param_defaults(state);
  from_file_version(raw_file_version, old_major, old_minor);

  // Map stored id to index for new params.
  std::map<stored_param_id, std::int32_t> new_params;
  for (std::int32_t rp = 0; rp < topology.input_param_count; rp++)
  {
    stored_param_id new_id;
    std::int32_t part_index = topology.params[rp].part_index;
    auto const& param = topology.params[rp].descriptor;
    auto const& part = topology.parts[part_index];
    new_id.param_guid = param->guid;
    new_id.part_index = part.type_index;
    new_id.io_type = param->data.io_type();
    new_id.part_guid = part.descriptor->guid;
    new_params[new_id] = rp;
  }

  std::set<stored_param_id> old_parameters;
  for (std::int32_t sp = 0; sp < param_count; sp++)
  {
    if(!read_string(part_guid)) return false;
    if(!read_int32(type_index)) return false;
    if(!read_string(param_guid)) return false;
    if(!read_int32(io_type)) return false;
    if(io_type < 0 || io_type >= param_io::count) return false;

    // Store old identifier. We'll remove it once matched.
    stored_param_id old_id;
    old_id.io_type = io_type;
    old_id.part_guid = part_guid;
    old_id.param_guid = param_guid;
    old_id.part_index = type_index;
    old_parameters.insert(old_id);

    str_value.clear();
    value = param_value();

    switch (io_type)
    {
    case param_io::real: 
      if (!read_float(value.real)) return false;
      break;
    case param_io::text:
      if(!read_string(str_value)) return false;
      break;
    case param_io::discrete: 
      if(!read_int32(value.discrete)) return false;
      break;
    default: assert(false); break;
    }

    auto new_id_iter = new_params.find(old_id);
    if(new_id_iter == new_params.end()) 
      continue;
    old_parameters.erase(new_id_iter->first);
    std::int32_t rp = new_id_iter->second;
    std::int32_t part_index = topology.params[rp].part_index;
    auto const& param = topology.params[rp].descriptor;

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

  // Report what's unused by new.
  for (auto it = old_parameters.begin(); it != old_parameters.end(); ++it)
    std::cout << "Unused value for " << it->part_guid << " " << it->part_index << ": " << it->param_guid << std::endl;
  topology.state_check(state);
  return true;
}
  
} // namespace inf::base