#include <inf.base/topology/topology_info.hpp>

#include <set>
#include <memory>
#include <cassert>

namespace inf::base {

// https://stackoverflow.com/questions/2351087/what-is-the-best-32bit-hash-function-for-short-strings-tag-names 
// std::hash is not required to be stable across program invocations.
// Note: required to be nonnegative by vst3.
static std::int32_t 
stable_hash(char const* str)
{ 
  std::uint32_t h = 0;
  std::int32_t const multiplier = 33;
  auto ustr = reinterpret_cast<std::uint8_t const*>(str);
  for (std::uint8_t const* p = ustr; *p != '\0'; p++) h = multiplier * h + *p;
  return std::abs(static_cast<std::int32_t>(h + (h >> 5)));
}

bool 
operator<(stored_param_id const& l, stored_param_id const& r)
{
  if (l.io_type < r.io_type) return true;
  if (l.io_type > r.io_type) return false;
  if (l.part_index < r.part_index) return true;
  if (l.part_index > r.part_index) return false;
  if (l.part_guid < r.part_guid) return true;
  if (l.part_guid > r.part_guid) return false;
  if (l.param_guid < r.param_guid) return true;
  if (l.param_guid > r.param_guid) return false;
  return false;
}

void 
topology_info::set_ui_value(
  param_value* state, std::int32_t part_type, 
  std::int32_t part_index, std::int32_t param, char const* value) const
{
  std::int32_t index = param_bounds[part_type][part_index] + param;
  state[index] = params[index].descriptor->data.parse_ui(part_index, value);
}

param_value
topology_info::ui_to_base_value(std::int32_t param_index, param_value ui_value) const
{
  param_value result = ui_value;
  auto const& desc = params[param_index];
  if (desc.descriptor->data.type == param_type::real)
    result.real = desc.descriptor->data.real.display.from_range(ui_value.real);
  return result;
}

param_value
topology_info::base_to_ui_value(std::int32_t param_index, param_value base_value) const
{
  param_value result = base_value;
  auto const& desc = params[param_index];
  if (desc.descriptor->data.type == param_type::real)
    result.real = desc.descriptor->data.real.display.to_range(base_value.real);
  return result;
}

void 
topology_info::init_param_defaults(param_value* state, std::int32_t from, std::int32_t to) const
{
  assert(from >= 0);
  assert(to >= from);
  assert(input_param_count >= to);
  for (std::int32_t p = from; p < to; p++)
    switch (params[p].descriptor->data.type)
    {
    case param_type::real: state[p].real = params[p].descriptor->data.real.default_; break;
    default: state[p].discrete = params[p].descriptor->data.discrete.default_; break;
    }
}

void
topology_info::state_check(param_value const* state) const
{
  for (std::int32_t p = 0; p < input_param_count; p++)
  {
    auto const& descriptor = *params[p].descriptor;
    if (descriptor.data.type == param_type::real)
      assert(0.0 <= state[p].real && state[p].real <= 1.0);
    else
    {
      std::int32_t val = state[p].discrete;
      (void)val;
      assert(descriptor.data.discrete.min <= val && val <= descriptor.data.discrete.max);
    }
  }
}
  
// Basically just cram as much stuff as we can that can be statically determined
// into the topology. Lots of index arrays, saves some loops in the processing call.
void
topology_info::init(
  topology_info* topology, part_descriptor const* static_parts,
  std::int32_t part_count, std::int32_t max_note_events)
{
  assert(part_count > 0);
  assert(max_note_events >= 0);
  assert(static_parts != nullptr);     

  topology->input_param_count = 0;
  topology->output_param_count = 0;
  topology->static_parts = static_parts;
  topology->static_part_count = part_count;
  topology->max_note_events = max_note_events;

  bool seen_output = false;
  std::int32_t part_index = 0;
  std::int32_t param_index = 0;
  for (std::int32_t t = 0; t < part_count; t++)
  {             
    assert(!seen_output || static_parts[t].kind == part_kind::output);
    seen_output |= static_parts[t].kind == part_kind::output;

    std::int32_t type_index = 0;
    topology->part_bounds.push_back(std::vector<std::int32_t>());
    topology->param_bounds.push_back(std::vector<std::int32_t>());
    std::string type_name(static_parts[t].static_name.short_);
    for (std::int32_t i = 0; i < static_parts[t].part_count; i++)
    {   
      topology->part_bounds[t].push_back(part_index);
      topology->param_bounds[t].push_back(param_index);
      std::string runtime_name = type_name;
      if(static_parts[t].kind == part_kind::output) topology->output_param_count += static_parts[t].param_count;
      else topology->input_param_count += static_parts[t].param_count;
      if (static_parts[t].part_count > 1) runtime_name += " " + std::to_string(i + 1);
      topology->parts.push_back(part_info({ type_index++, runtime_name, param_index, &static_parts[t] }));
      part_index++;
      param_index += static_parts[t].param_count;
    }
  }

  for (std::int32_t part = 0; part < topology->parts.size(); part++)
  {
    auto const& part_info = topology->parts[part];
    std::string part_name = part_info.runtime_name;
    for (std::int32_t p = 0; p < part_info.descriptor->param_count; p++)
    {
      auto const& descriptor = topology->parts[part].descriptor->params[p];
      assert(!descriptor.data.is_continuous() || descriptor.data.type == param_type::real);
      assert((part_info.descriptor->kind != part_kind::selector) || descriptor.data.kind == param_kind::ui);
      assert((part_info.descriptor->kind == part_kind::output) == (descriptor.data.kind == param_kind::output));
      std::string runtime_name = part_name + " " + descriptor.data.static_name.detail;
      topology->params.push_back(param_info({ part, runtime_name, &descriptor }));
      topology->inverse_bounds.push_back({ part_info.descriptor->type, part_info.type_index });
    }
  }

  for (std::size_t p = 0; p < topology->params.size(); p++)
  {
    auto const& this_param = topology->params[p];
    auto const& this_descriptor = *this_param.descriptor;
    if(this_descriptor.data.is_continuous()) topology->continuous_param_count++;

    // Index<>external ID mapping.
    auto const& part = topology->parts[topology->params[p].part_index];
    std::string unique_param_id = part.descriptor->guid;
    unique_param_id += std::to_string(part.type_index);
    unique_param_id += this_descriptor.guid;    
    std::int32_t hash = stable_hash(unique_param_id.c_str()); 
    topology->param_index_to_id.push_back(hash);
    assert(topology->param_id_to_index.find(hash) == topology->param_id_to_index.end());
    topology->param_id_to_index[hash] = static_cast<std::int32_t>(p);
  }

  // Check for duplicate guids.
  std::set<std::string> seen_parts;
  std::set<std::string> seen_params;
  for(std::int32_t i = 0; i < topology->static_part_count; i++)
  {
    auto const& part = topology->static_parts[i];
    assert(seen_parts.insert(part.guid).second);
    for (std::int32_t j = 0; j < part.param_count; j++)
      assert(seen_params.insert(part.params[j].guid).second);
  }  

  // Make sure all DSP bounds are linear bounded otherwise CLAP behaves funny.
  for (std::size_t i = 0; i < topology->params.size(); i++)
  {
    auto const& param_data = topology->params[i].descriptor->data;
    (void)param_data;
    assert(param_data.type != param_type::real ||
      (param_data.real.dsp.slope == real_slope::linear &&
       param_data.real.dsp.min > -std::numeric_limits<float>::infinity() &&
       param_data.real.dsp.max < std::numeric_limits<float>::infinity()));
  }
   
  assert(topology->parts.size() > 0);
  assert(topology->params.size() > 0);
  assert(topology->part_bounds.size() > 0);
  assert(topology->param_bounds.size() > 0);
  assert(topology->params.size() == topology->input_param_count + topology->output_param_count);
} 

} // namespace inf::base