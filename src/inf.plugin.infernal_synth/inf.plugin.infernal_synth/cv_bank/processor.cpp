#include <inf.base/shared/support.hpp>
#include <inf.plugin.infernal_synth/cv_bank/processor.hpp>

#include <algorithm>

using namespace inf::base;

namespace inf::plugin::infernal_synth {

// Global.
cv_bank_processor::
cv_bank_processor(topology_info const* topology, cv_bank_state* state):
_state(state), _data(&cv_bank_data::global), _topology(topology),
_relevant_indices_count(), _relevant_indices()
{
  assert(state != nullptr);
  assert(topology != nullptr);
}

// Voice.
cv_bank_processor::
cv_bank_processor(topology_info const* topology, cv_bank_state* state, 
  cv_hold_sample const* gcv_uni_hold_, cv_hold_sample const* gcv_bi_hold_, 
  cv_hold_sample const* glfo_hold_, float velo, std::int32_t midi, base::block_input_data const& input):
_state(state), _data(&cv_bank_data::voice), _topology(topology),
_relevant_indices_count(), _relevant_indices()
{ 
  assert(state != nullptr);
  assert(topology != nullptr);
  assert(gcv_uni_hold_ != nullptr);
  assert(gcv_bi_hold_ != nullptr);
  assert(glfo_hold_ != nullptr);
  update_block_params(input);
  apply_voice_state(gcv_uni_hold_, gcv_bi_hold_, glfo_hold_, velo, midi, input.sample_count);
}

float const*
cv_bank_processor::input_buffer(std::int32_t input, std::int32_t index) const
{
  switch (_data->part_type)
  {
  case part_type::vcv_bank: return input_buffer_voice(input, index);
  case part_type::gcv_bank: return input_buffer_global(input, index);
  default: assert(false); return nullptr;
  }
}

bool
cv_bank_processor::input_bipolar(std::int32_t input, std::int32_t index) const
{
  switch(_data->part_type)
  {
  case part_type::vcv_bank: return input_bipolar_voice(input, index);
  case part_type::gcv_bank: return input_bipolar_global(input, index);
  default: assert(false); return false;
 }
}

float const*
cv_bank_processor::input_buffer_global(std::int32_t input, std::int32_t index) const
{
  switch (input)
  {
  case gcv_route_input::off: assert(false); return nullptr;
  case gcv_route_input::glfo: return _state->glfo[index].buffer.values;
  case gcv_route_input::gcv_bi: return _state->gcv_bi[index].buffer.values;
  case gcv_route_input::gcv_uni: return _state->gcv_uni[index].buffer.values;
  case gcv_route_input::midi_ch_vol: return _state->midi_ch_vol.data();
  case gcv_route_input::midi_ch_press: return _state->midi_ch_press.data();
  case gcv_route_input::midi_mod_wheel: return _state->midi_mod_wheel.data();
  case gcv_route_input::midi_pitch_bend: return _state->midi_pitch_bend.data();
  default: assert(false); return nullptr;
  }
}

float const*
cv_bank_processor::input_buffer_voice(std::int32_t input, std::int32_t index) const
{
  switch (input)
  {
  case vcv_route_input::off: assert(false); return nullptr;
  case vcv_route_input::key: return _state->key.data();
  case vcv_route_input::velo: return _state->velo.data();
  case vcv_route_input::key_inv: return _state->key_inv.data();
  case vcv_route_input::midi_ch_vol: return _state->midi_ch_vol.data();
  case vcv_route_input::midi_ch_press: return _state->midi_ch_press.data();
  case vcv_route_input::midi_mod_wheel: return _state->midi_mod_wheel.data();
  case vcv_route_input::midi_pitch_bend: return _state->midi_pitch_bend.data();

  case vcv_route_input::vlfo: return _state->vlfo[index].buffer.values;
  case vcv_route_input::venv: return _state->venv[index].buffer.values;
  case vcv_route_input::glfo: return _state->glfo[index].buffer.values;
  case vcv_route_input::gcv_bi: return _state->gcv_bi[index].buffer.values;
  case vcv_route_input::gcv_uni: return _state->gcv_uni[index].buffer.values;
  case vcv_route_input::glfo_hold: return _state->glfo_hold[index].buffer.values;
  case vcv_route_input::gcv_bi_hold: return _state->gcv_bi_hold[index].buffer.values;
  case vcv_route_input::gcv_uni_hold: return _state->gcv_uni_hold[index].buffer.values;
  default: assert(false); return nullptr;
  }
}

bool
cv_bank_processor::input_bipolar_global(std::int32_t input, std::int32_t index) const
{
  switch (input)
  {
  case gcv_route_input::off: return false;
  case gcv_route_input::gcv_bi: return true;
  case gcv_route_input::gcv_uni: return false;
  case gcv_route_input::midi_ch_vol: return false;
  case gcv_route_input::midi_ch_press: return false;
  case gcv_route_input::midi_mod_wheel: return false;
  case gcv_route_input::midi_pitch_bend: return true;
  case gcv_route_input::glfo: return _state->glfo[index].buffer.flags.bipolar;
  default: assert(false); return false;
  }
}

bool
cv_bank_processor::input_bipolar_voice(std::int32_t input, std::int32_t index) const
{
  switch (input)
  {
  case vcv_route_input::off: return false;
  case vcv_route_input::key: return false;
  case vcv_route_input::velo: return false;
  case vcv_route_input::key_inv: return false;
  case vcv_route_input::midi_ch_vol: return false;
  case vcv_route_input::midi_ch_press: return false;
  case vcv_route_input::midi_mod_wheel: return false;
  case vcv_route_input::midi_pitch_bend: return true;

  case vcv_route_input::gcv_bi: return true;
  case vcv_route_input::gcv_uni: return false;
  case vcv_route_input::gcv_bi_hold: return true;
  case vcv_route_input::gcv_uni_hold: return false;
  case vcv_route_input::vlfo: return _state->vlfo[index].buffer.flags.bipolar;
  case vcv_route_input::venv: return _state->venv[index].buffer.flags.bipolar;
  case vcv_route_input::glfo: return _state->glfo[index].buffer.flags.bipolar;
  case vcv_route_input::glfo_hold: return _state->glfo_hold[index].buffer.flags.bipolar;
  default: assert(false); return 0.0f;
  }
}

void
cv_bank_processor::apply_voice_state(
  cv_hold_sample const* gcv_uni_hold, cv_hold_sample const* gcv_bi_hold,
  cv_hold_sample const* glfo_hold, float velo, std::int32_t midi, std::int32_t sample_count)
{
  midi = std::clamp(midi, 0, 127);
  std::fill(_state->velo.data(), _state->velo.data() + sample_count, velo);
  std::fill(_state->key.data(), _state->key.data() + sample_count, static_cast<float>(midi) / 127.0f);
  std::fill(_state->key_inv.data(), _state->key_inv.data() + sample_count, 1.0f - static_cast<float>(midi) / 127.0f);
  for (std::int32_t i = 0; i < glfo_count; i++)
  {
    _state->glfo_hold[i].buffer.flags = glfo_hold[i].flags;
    std::fill(_state->glfo_hold[i].buffer.values, _state->glfo_hold[i].buffer.values + sample_count, glfo_hold[i].value);
  }
  for (std::int32_t i = 0; i < master_gcv_count; i++)
  {
    _state->gcv_bi_hold[i].buffer.flags = gcv_bi_hold[i].flags;
    _state->gcv_uni_hold[i].buffer.flags = gcv_uni_hold[i].flags;
    std::fill(_state->gcv_bi_hold[i].buffer.values, _state->gcv_bi_hold[i].buffer.values + sample_count, gcv_bi_hold[i].value);
    std::fill(_state->gcv_uni_hold[i].buffer.values, _state->gcv_uni_hold[i].buffer.values + sample_count, gcv_uni_hold[i].value);
  }
}

void
cv_bank_processor::update_block_params(block_input_data const& input)
{
  // Find out relevant modulation targets.
  _relevant_indices_count.fill(0);

  // cv_bank_param_offset = enabled + plot params
  part_id bank_id = { _data->part_type, 0 };
  automation_view bank_automation = input.automation.rearrange_params(bank_id);

  // For each route.
  for (std::int32_t r = 0; r < cv_bank_route_count; r++)
  {
    std::int32_t in_index = cv_bank_param_index(r, cv_bank_param_type::in);
    std::int32_t source_id = bank_automation.block_discrete(in_index);
    if (source_id == cv_bank_vgcv_inout_off) continue;

    // For each target.
    for (std::int32_t output_id = 0; output_id < _data->route_output_total_count; output_id++)
    {
      std::pair<std::int32_t, std::int32_t> output_ids = _data->output_table_out[output_id];
      std::int32_t route_output = output_ids.first;
      std::int32_t part_index = output_ids.second; 
      if(route_output == cv_bank_vgcv_inout_off) continue;

      // If matched, mark in/out combination as relevant for a given mixdown target.
      for (std::int32_t p = 0; p < _data->route_output_target_counts[route_output]; p++)
      {
        std::int32_t target_id = _data->target_table_in[route_output][part_index][p];
        if (target_id == cv_bank_vgcv_inout_off) continue;
        std::int32_t target_index = cv_bank_param_index(r, cv_bank_param_type::out);
        if (bank_automation.block_discrete(target_index) != target_id) continue;

        cv_route_indices indices;
        indices.route_index = r;
        indices.target_index = p;
        indices.input_ids = _data->source_table_out[source_id];
        indices.input_op_index = bank_automation.block_discrete(cv_bank_param_index(r, cv_bank_param_type::op));
        _relevant_indices[output_id][_relevant_indices_count[output_id]++] = indices;
      }
    }
  }
}

void
cv_bank_processor::apply_modulation(cv_bank_input const& input, 
  automation_view const& bank_automation, cv_route_indices const& indices, std::int32_t mapped_target)
{
  std::int32_t sample_count = input.block->sample_count;

  // Get modifiers.
  float* amt = _state->amt.data();
  float* scale = _state->scale.data();
  float* offset = _state->offset.data();
  std::int32_t amt_param = cv_bank_param_index(indices.route_index, cv_bank_param_type::amt);
  std::int32_t scale_param = cv_bank_param_index(indices.route_index, cv_bank_param_type::scale);
  std::int32_t offset_param = cv_bank_param_index(indices.route_index, cv_bank_param_type::off);
  bank_automation.continuous_real_transform(amt_param, amt, sample_count);
  bank_automation.continuous_real_transform(scale_param, scale, sample_count);
  bank_automation.continuous_real_transform(offset_param, offset, sample_count);

  // Get input signal.
  std::int32_t source_type = std::get<0>(indices.input_ids);
  std::int32_t source_index = std::get<1>(indices.input_ids);
  bool bipolar = input_bipolar(source_type, source_index);
  float const* in = input_buffer(source_type, source_index);
  float* out = _state->out.buffer(mapped_target);

  // Apply modifiers.
  // Bipolar input is transformed to unipolar first, then modified, then set back to bipolar.
  // For keyboard tracking, we scale outward rather than inward, to give user 100% range e.g. between c3 and c5.
  std::int32_t ss = sample_count;
  float* in_modified = _state->in_modified.data();
  if(_data->part_type == part_type::vcv_bank && (source_type == vcv_route_input::key || source_type == vcv_route_input::key_inv))
    for (std::int32_t s = 0; s < ss; s++)
    {
      float min_mod = offset[s];
      float max_mod = offset[s] + (1.0f - offset[s]) * scale[s];
      in_modified[s] = amt[s] * (in[s] < min_mod ? 0.0f: in[s] > max_mod ? 1.0f: (in[s] - min_mod) / (max_mod - min_mod));
    }
  else if (!bipolar)
    for (std::int32_t s = 0; s < ss; s++)
      in_modified[s] = amt[s] * (offset[s] + (1.0f - offset[s]) * scale[s] * in[s]);
  else
    for (std::int32_t s = 0; s < ss; s++)
      in_modified[s] = amt[s] * ((offset[s] + (1.0f - offset[s]) * scale[s] * (in[s] + 1.0f) * 0.5f) * 2.0f - 1.0f);

  for (std::int32_t s = 0; s < ss; s++)
  {
    assert(bipolar || (-sanity_epsilon <= in_modified[s] && in_modified[s] <= 1.0f + sanity_epsilon));
    assert(!bipolar || (-1.0f - sanity_epsilon <= in_modified[s] && in_modified[s] <= 1.0f + sanity_epsilon));
  }

  // Modulate.
  switch (indices.input_op_index)
  {
  case cv_route_input_op::add:
    if (!bipolar) for (std::int32_t s = 0; s < ss; s++) out[s] += (1.0f - out[s]) * in_modified[s];
    else for (std::int32_t s = 0; s < ss; s++) out[s] += std::min(out[s], 1.0f - out[s]) * in_modified[s];
    break;
  case cv_route_input_op::sub:
    if (!bipolar) for (std::int32_t s = 0; s < ss; s++) out[s] -= out[s] * in_modified[s];
    else for (std::int32_t s = 0; s < ss; s++) out[s] -= std::min(out[s], 1.0f - out[s]) * in_modified[s];
    break;
  case cv_route_input_op::mul:
    if (!bipolar) for (std::int32_t s = 0; s < ss; s++) out[s] = (1.0f - amt[s] + in_modified[s]) * out[s];
    else for (std::int32_t s = 0; s < ss; s++) out[s] = (1.0f - amt[s]) * out[s] + out[s] * std::abs(in_modified[s]);
    break;
  default:
    assert(false);
    break;
  }
}

std::int64_t 
cv_bank_processor::modulate(cv_bank_input const& input, float const* const*& result)
{
  result = _state->out.buffers();
  std::int64_t start_time = performance_counter();
  std::int32_t mod_type = input.modulation_id.type;
  std::int32_t mod_index = input.modulation_id.index;
  std::int32_t sample_count = input.block->sample_count;
  std::int32_t output_id = _data->output_table_in[mod_type][mod_index];
  std::int32_t relevant_indices_count = _relevant_indices_count[output_id];
  std::int32_t const* target_mapping = _data->route_output_target_mapping[mod_type];

  // Shortcut, need no modulation.
  if (relevant_indices_count == 0)
  {
    for (std::int32_t p = 0; p < _data->route_output_target_counts[mod_type]; p++)
      input.automated->continuous_real_transform(target_mapping[p], _state->out.buffer(target_mapping[p]), sample_count);
    return performance_counter() - start_time;
  }

  // Set up all outputs for part type + index (in [0, 1], normalized parameter values). 
  // Even if not modulated, all will be transformed to dsp range.
  for (std::int32_t p = 0; p < _data->route_output_target_counts[mod_type]; p++)
    input.automated->continuous_real(target_mapping[p], _state->out.buffer(target_mapping[p]), sample_count);

  // Apply modulation (can scale beyond [0, 1], we apply clipping just before transform to dsp domain).
  part_id bank_id = { _data->part_type, 0 };
  automation_view bank_automation = input.block->automation.rearrange_params(bank_id);
  for (std::int32_t i = 0; i < relevant_indices_count; i++)
  {
    cv_route_indices indices = _relevant_indices[output_id][i];
    std::int32_t mapped_target = target_mapping[indices.target_index];
    apply_modulation(input, bank_automation, indices, mapped_target);
  }

  // Clamp modulated stuff to [0, 1].
  std::fill(_state->clamped.begin(), _state->clamped.end(), 0);
  for (std::int32_t i = 0; i < relevant_indices_count; i++)
  {
    cv_route_indices indices = _relevant_indices[output_id][i];
    if(_state->clamped[indices.target_index]) continue;
    _state->clamped[indices.target_index] = 1;
    float* out = _state->out.buffer(target_mapping[indices.target_index]);
    for (std::int32_t s = 0; s < sample_count; s++)
      out[s] = std::clamp(out[s], 0.0f, 1.0f);
  }

  // Scale [0, 1] to dsp. Note: scale everything, also stuff that wasn't modulated.
  // On end of transform(), each series should be in it's own domain for dsp processing.
  for (std::int32_t p = 0; p < _data->route_output_target_counts[mod_type]; p++)
  {
    float* out = _state->out.buffer(target_mapping[p]);
    for(std::int32_t s = 0; s < sample_count; s++)
      sanity_unipolar(out[s]);
    input.automated->continuous_transform(target_mapping[p], out, sample_count);
  } 

  return performance_counter() - start_time;
}

} // namespace inf::plugin::infernal_synth