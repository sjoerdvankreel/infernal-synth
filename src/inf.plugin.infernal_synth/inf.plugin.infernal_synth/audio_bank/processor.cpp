#include <inf.base/shared/support.hpp>
#include <inf.plugin.infernal_synth/shared/support.hpp>
#include <inf.plugin.infernal_synth/cv_bank/processor.hpp>
#include <inf.plugin.infernal_synth/audio_bank/processor.hpp>

using namespace inf::base;

namespace inf::plugin::infernal_synth {

float const* const*
audio_bank_processor::input_buffer(std::int32_t input, std::int32_t index) const
{
  switch(_data->part_type)
  {
  case part_type::vaudio_bank: return input_buffer_voice(input, index);
  case part_type::gaudio_bank: return input_buffer_global(input, index); 
  default: assert(false); return nullptr;
  }
}

float const* const*
audio_bank_processor::input_buffer_voice(std::int32_t input, std::int32_t index) const
{
  switch (input)
  {
  case vaudio_route_input::off: assert(false); return nullptr;
  case vaudio_route_input::vosc_all: return _state->vosc_all.buffers();
  case vaudio_route_input::vosc_any: return _state->vosc[index].buffers();
  case vaudio_route_input::veffect: return _state->veffect[index].buffers();
  default: assert(false); return nullptr;
  }
} 

float const* const*
audio_bank_processor::input_buffer_global(std::int32_t input, std::int32_t index) const
{
  switch (input)
  {
  case gaudio_route_input::off: assert(false); return nullptr;
  case gaudio_route_input::voice: return _state->voices.buffers();
  case gaudio_route_input::external: return _state->external.buffers();
  case gaudio_route_input::geffect: return _state->geffect[index].buffers();
  default: assert(false); return nullptr;
  }
} 

void 
audio_bank_processor::update_block_params(block_input_data const& input)
{
  // Find out relevant audio targets.
  _relevant_indices_count.fill(0);

  // For each bank.
  part_id bank_id = { _data->part_type, 0 };
  automation_view bank_automation = input.automation.rearrange_params(bank_id);

  // For each route.
  for (std::int32_t r = 0; r < audio_bank_route_count; r++)
  {
    std::int32_t in_index = audio_bank_param_index(r, audio_bank_param_type::in);
    std::int32_t input_id = bank_automation.block_discrete(in_index);
    if (input_id == audio_bank_vgaudio_inout_off) continue;

    // For each target.
    for (std::int32_t output_id = 0; output_id < _data->total_route_count; output_id++)
    {
      if (output_id == audio_bank_vgaudio_inout_off) continue;
      std::int32_t out_index = audio_bank_param_index(r, audio_bank_param_type::out);
      std::int32_t this_output_id = bank_automation.block_discrete(out_index);
      if (output_id != this_output_id) continue;

      // If matched, mark in/out combination as relevant for a given mixdown target.
      audio_route_indices indices;
      indices.route_index = r;
      indices.input_ids = _data->input_table_out[input_id];
      _relevant_indices[output_id][_relevant_indices_count[output_id]++] = indices;
    }
  }
}

audio_bank_output
audio_bank_processor::process(block_input_data const& input, part_id id, cv_bank_processor& cv)
{
  automation_view bank_automation;
  float const* const* params = nullptr;
  _state->out.clear(input.sample_count);

  audio_bank_output result = { };
  std::int64_t start_time = performance_counter();
  std::int32_t output_id = _data->output_table_in[id.type][id.index];

  // Apply cv modulation.
  cv_bank_input cv_input;
  part_id bank_id = { _data->part_type, 0 };
  bank_automation = input.automation.rearrange_params(bank_id);
  cv_input.block = &input;
  cv_input.automated = &bank_automation;
  cv_input.modulation_id = { _data->modulation_type, 0 };
  result.cv_time += cv.modulate(cv_input, params);

  // For each route that wants to add audio to this target.
  for (std::int32_t i = 0; i < _relevant_indices_count[output_id]; i++)
  {
    // Need to apply cv modulation to each audio bank separately.
    audio_route_indices indices = _relevant_indices[output_id][i];

    // Add audio to mixdown, taking amplitude and balance into account.
    float* const* out = _state->out.buffers();
    float const* amt = params[audio_bank_param_index(indices.route_index, audio_bank_param_type::amt)];
    float const* bal = params[audio_bank_param_index(indices.route_index, audio_bank_param_type::bal)];
    float const* const* in = input_buffer(indices.input_ids.first, indices.input_ids.second);
    for(std::int32_t c = 0; c < stereo_channels; c++)
      for (std::int32_t s = 0; s < input.sample_count; s++)
        out[c][s] = sanity(out[c][s] + in[c][s] * amt[s] * stereo_balance(c, bal[s]));
  }

  result.mixdown = _state->out.buffers(); 
  result.own_time = performance_counter() - start_time - result.cv_time;
  return result;
}

} // namespace inf::plugin::infernal_synth 