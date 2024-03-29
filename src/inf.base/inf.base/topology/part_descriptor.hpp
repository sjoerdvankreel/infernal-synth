#ifndef INF_BASE_TOPOLOGY_PART_DESCRIPTOR_HPP
#define INF_BASE_TOPOLOGY_PART_DESCRIPTOR_HPP

#include <inf.base/shared/state.hpp>
#include <inf.base/topology/param_descriptor.hpp>
#include <cstdint>

namespace inf::base {

struct topology_info;

struct part_id
{
  std::int32_t type;
  std::int32_t index;
};

typedef char const* (*graph_name_selector)(
topology_info const* topology, param_value const* state, part_id id, std::int32_t graph_type);

// Input: (optionally automatable) input parts.
// Selector: (optionally automatable) selector (in case of part types with more than 1 instance). At most one.
// Output: not-automatable output parameters.
// Note: outputs must go last and can contain output params only.
struct part_kind_t { enum value { input, selector, output, count }; };
typedef part_kind_t::value part_kind;

// Static part info.
struct part_descriptor
{
  std::string const guid; // Unique part id for serialization.
  item_name const static_name; // Static name, e.g. "Osc", "Lfo".
  part_kind const kind; // Input/selector/output.
  std::int32_t const type; // Type id, e.g. Osc, Lfo.
  std::int32_t const part_count; // Part count of this type, e.g. 2 lfos.
  param_descriptor const* const params; // Pointer to parameter descriptor array.
  std::int32_t const param_count; // Parameter count for a part of this type, e.g. 2: frequency, resonance.
  graph_name_selector name_selector; // Name may depend on part state.
};

} // namespace inf::base
#endif // INF_BASE_TOPOLOGY_PART_DESCRIPTOR_HPP