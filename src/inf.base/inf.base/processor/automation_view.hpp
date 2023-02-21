#ifndef INF_BASE_SHARED_AUTOMATION_VIEW_HPP
#define INF_BASE_SHARED_AUTOMATION_VIEW_HPP

#include <inf.base/topology/topology_info.hpp>

#include <cassert>
#include <cstdint>
#include <algorithm>

namespace inf::base {

// View into automation buffer.
class automation_view
{
  std::int32_t _sample_count = 0;
  std::int32_t _sample_offset = 0;

  // Allow for polyphonic synth previous voices to fix automation 
  // values at the release stage when a new note begins.
  std::int32_t _sample_fixed_at = 0;

  std::int32_t _total_param_count = 0;
  std::int32_t _part_param_count = 0;
  std::int32_t _part_param_offset = 0;

  topology_info const* _topology = nullptr;
  param_value const* _block = nullptr;
  float const* const* _continuous = nullptr;
  float const* _fixed = nullptr;

  param_descriptor const* param_at(std::int32_t index) const;
  void continuous_checks(std::int32_t param, std::int32_t count) const;

public:
  automation_view() = default;
  
  // continuous_real takes care of _fixed.
  // continuous_transform assumes _fixed has already been taken care of.
  // real_transform does both in one go, falls back to real + transform in case of _fixed.
  float block_real_transform(std::int32_t param) const;
  std::int32_t block_discrete(std::int32_t param) const;
  void continuous_real(std::int32_t param, float* out, std::int32_t count) const;
  void continuous_transform(std::int32_t param, float* inout, std::int32_t count) const;
  void continuous_real_transform(std::int32_t param, float* out, std::int32_t count) const;

  // Rearrange part or sample indices.
  automation_view rearrange_params(part_id id) const;
  automation_view rearrange_samples(std::int32_t sample_offset, std::int32_t sample_fixed_at) const;

  // For graph component.
  float from_dsp(std::int32_t param, float val) const;

  // Initial construction using all params.
  automation_view(
    topology_info const* topology, param_value const* block, 
    float const* const* continuous, float const* fixed,
    std::int32_t total_param_count, std::int32_t part_param_count, 
    std::int32_t part_param_offset, std::int32_t sample_count, 
    std::int32_t sample_offset, std::int32_t sample_fixed_at);

  // Only for block type values.
  automation_view(topology_info const* topology, param_value const* block, part_id id);
};

// Only for block type values.
// Arranges param indices during construction.
inline automation_view::
automation_view(
topology_info const* topology, param_value const* state, part_id id):
_sample_count(0), _sample_offset(0), _sample_fixed_at(0), 
_total_param_count(topology->input_param_count),
_part_param_count(topology->static_parts[id.type].param_count), 
_part_param_offset(topology->param_start(id)),
_topology(topology), _block(state), 
_continuous(nullptr), _fixed(nullptr)
{
  assert(state != nullptr);
  assert(topology != nullptr);
}

inline automation_view::
automation_view(
  topology_info const* topology, param_value const* block, 
  float const* const* continuous, float const* fixed,
  std::int32_t total_param_count, std::int32_t part_param_count, 
  std::int32_t part_param_offset, std::int32_t sample_count, 
  std::int32_t sample_offset, std::int32_t sample_fixed_at):
_sample_count(sample_count), _sample_offset(sample_offset), 
_sample_fixed_at(sample_fixed_at), _total_param_count(total_param_count), 
_part_param_count(part_param_count), _part_param_offset(part_param_offset),
_topology(topology), _block(block), _continuous(continuous), _fixed(fixed)
{
  assert(block != nullptr);
  assert(topology != nullptr);
  assert(part_param_count > 0);
  assert(total_param_count > 0);
  assert(part_param_offset >= 0);
  assert(part_param_offset < total_param_count);
  assert(total_param_count >= part_param_count);
  assert(sample_offset >= 0);
  assert(sample_fixed_at >= 0);
  assert(continuous == nullptr || sample_count > 0);
  assert(continuous == nullptr || sample_offset < sample_count);
  assert(sample_fixed_at <= sample_count - sample_offset);
  assert(sample_fixed_at == sample_count - sample_offset || fixed != nullptr);
}

inline param_descriptor const* 
automation_view::param_at(std::int32_t index) const
{ return _topology->params[index + _part_param_offset].descriptor; }

inline std::int32_t
automation_view::block_discrete(std::int32_t param) const
{
  assert(param >= 0);
  assert(param < _part_param_count);
  assert(param_at(param)->data.type != param_type::real);
  return _block[param + _part_param_offset].discrete;
}

// For graph component.
inline float
automation_view::from_dsp(std::int32_t param, float val) const
{
  assert(param >= 0);
  assert(param < _part_param_count);
  return param_at(param)->data.real.dsp.from_range(val);
}

inline void
automation_view::continuous_checks(std::int32_t param, std::int32_t count) const
{
  assert(param >= 0);
  assert(param < _part_param_count);
  assert(count >= 0);
  assert(count <= _sample_count - _sample_offset);
  assert(param_at(param)->data.is_continuous());
  assert(param_at(param)->data.type == param_type::real);
}

// Treat continuous params as block.
inline float
automation_view::block_real_transform(std::int32_t param) const
{
  assert(param >= 0);
  assert(param < _part_param_count);
  assert(param_at(param)->data.type == param_type::real);
  return param_at(param)->data.real.dsp.to_range(_block[param + _part_param_offset].real);
}

inline automation_view
automation_view::rearrange_params(part_id id) const
{
  std::int32_t param_count = _topology->static_parts[id.type].param_count;
  std::int32_t param_offset = _topology->param_start(id);
  return automation_view(
    _topology, _block, _continuous, _fixed,
    _total_param_count, param_count, param_offset,
    _sample_count, _sample_offset, _sample_fixed_at);
}

inline automation_view
automation_view::rearrange_samples(std::int32_t sample_offset, std::int32_t sample_fixed_at) const
{
  return automation_view(
    _topology, _block, _continuous, _fixed,
    _total_param_count, _part_param_count, _part_param_offset,
    _sample_count, sample_offset, sample_fixed_at);
}

inline void
automation_view::continuous_transform(std::int32_t param, float* inout, std::int32_t count) const
{
  continuous_checks(param, count);
  param_at(param)->data.real.dsp.to_range(inout, count);
}

inline void
automation_view::continuous_real_transform(std::int32_t param, float* out, std::int32_t count) const
{
  continuous_checks(param, count);
  if (_fixed == nullptr)
  {
    auto const& bounds = param_at(param)->data.real.dsp;
    bounds.to_range(_continuous[param + _part_param_offset], out, count);
    return;
  }
  continuous_real(param, out, count);
  continuous_transform(param, out, count);
}

inline void
automation_view::continuous_real(std::int32_t param, float* out, std::int32_t count) const
{
  continuous_checks(param, count);
  std::int32_t automated_count = std::min(count, _sample_fixed_at);
  float const* automated = _continuous[param + _part_param_offset];
  std::copy(automated, automated + automated_count, out);
  std::int32_t fixed_count = std::max(0, count - _sample_fixed_at);
  assert(fixed_count == 0 || _fixed != nullptr);
  if (fixed_count > 0) std::fill(out + automated_count, out + automated_count + fixed_count, _fixed[_part_param_offset + param]);
}

} // namespace inf::base
#endif // INF_BASE_SHARED_AUTOMATION_VIEW_HPP