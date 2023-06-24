#include <inf.synth/effect/topology.hpp>
#include <inf.synth/effect/processor.hpp>
#include <inf.synth/cv_bank/processor.hpp>
#include <inf.synth/effect/processor_shaper.hpp>

#include <cassert>

using namespace inf::base;

namespace inf::synth {

static std::int32_t 
delay_sample_count(automation_view const& automation, float rate, float bpm, effect_param time, effect_param tempo)
{
  std::int32_t result;
  float timesig = effect_dly_timesig_values[automation.block_discrete(tempo)];
  if (automation.block_discrete(effect_param::dly_synced) == 1)
    result = static_cast<std::int32_t>(timesig_to_samples(rate, bpm, timesig));
  else
    result = static_cast<std::int32_t>(automation.block_real_transform(time) * rate);
  // Cut off rather then wraparound when buffer size is exceeded (e.g. when synced against low BPM).
  return std::max(1, std::min(result, static_cast<std::int32_t>(std::ceil(rate * effect_dly_max_time_sec - 1.0f))));
}

effect_processor::
effect_processor(topology_info const* topology, part_id id, float sample_rate, 
float bpm, std::int32_t midi, effect_state* state, automation_view const& automation):
audio_part_processor(topology, id, sample_rate, modulation_type(id.type)),
_midi(midi), _state(state), _on(), _type(), _delay_type(), _filter_type(), _shaper_type(), 
_flt_stvar_type(), _shp_over_order(), _shp_cheby_terms(), _dly_multi_taps(), _dly_multi_length()
{
   assert(state != nullptr);
   assert((id.type == part_type::geffect) == (state->global));
   state->reset();
   update_block_params(automation, midi, bpm);
}

void
effect_processor::update_block_params(automation_view const& automation, std::int32_t midi, float bpm)
{
  update_flt_stvar_kbd_track(midi); 
  automation_view eff_automation(automation.rearrange_params(id()));
  _on = eff_automation.block_discrete(effect_param::on);
  _type = eff_automation.block_discrete(effect_param::type);
  _filter_type = eff_automation.block_discrete(effect_param::filter_type);
  _shaper_type = eff_automation.block_discrete(effect_param::shaper_type);
  _flt_stvar_type = eff_automation.block_discrete(effect_param::flt_stvar_type);
  _shp_over_order = eff_automation.block_discrete(effect_param::shp_over_order);
  _shp_cheby_terms = eff_automation.block_discrete(effect_param::shp_cheby_terms);

  if (id().type != part_type::geffect) return;

  // Delay and reverb are global fx only.
  std::int32_t delay_max_samples = static_cast<std::int32_t>(std::ceil(sample_rate() * effect_dly_max_time_sec));
  _delay_type = eff_automation.block_discrete(effect_param::delay_type);
  _dly_multi_taps = eff_automation.block_discrete(effect_param::dly_multi_taps);
  _dly_hold_length = delay_sample_count(eff_automation, sample_rate(), bpm, effect_param::dly_hold_time, effect_param::dly_hold_tempo);
  _dly_multi_length = delay_sample_count(eff_automation, sample_rate(), bpm, effect_param::dly_multi_time, effect_param::dly_multi_tempo);
  _dly_multi_taps = std::min(_dly_multi_taps, (delay_max_samples - 1 - _dly_hold_length) / _dly_multi_length);
  _state->dly_fdbk_length[0] = delay_sample_count(eff_automation, sample_rate(), bpm, effect_param::dly_fdbk_time_l, effect_param::dly_fdbk_tempo_l);
  _state->dly_fdbk_length[1] = delay_sample_count(eff_automation, sample_rate(), bpm, effect_param::dly_fdbk_time_r, effect_param::dly_fdbk_tempo_r);
}

audio_part_output
effect_processor::process_global(effect_input const& input, float* const* out, cv_bank_processor& cv, scratch_space& scratch)
{
  // Global level effect updates block params once per block.
  automation_view automation(input.block->automation.rearrange_params(id()));
  update_block_params(automation, input.new_midi, input.block->bpm);
  return process_any(input, out, cv, scratch);
}

audio_part_output
effect_processor::process_any(effect_input const& input, float* const* out, cv_bank_processor& cv, scratch_space& scratch)
{
  audio_part_output result = { 0, 0 };
  if (_on == 0)
  {
    for(std::int32_t c = 0; c < stereo_channels; c++)
      std::copy(input.audio_in[c], input.audio_in[c] + input.block->sample_count, out[c]);
    return result;
  }

  cv_bank_input cv_input;
  float const* const* params;
  automation_view automation(input.block->automation.rearrange_params(id()));
  cv_input.block = input.block;
  cv_input.automated = &automation;
  cv_input.modulation_id = modulation_id();
  result.cv_time = cv.modulate(cv_input, params);

  effect_process_input process_input;
  process_input.params = params;
  process_input.audio_in = input.audio_in;
  process_input.new_midi = input.new_midi;
  process_input.new_midi_start_pos = input.new_midi_start_pos;
  process_input.sample_count = input.block->sample_count;

  // Block process fx type.
  // Delay and reverb are supported for global fx only.
  std::int64_t start_time = performance_counter();
  switch (_type)
  { 
  case effect_type::delay: process_dly(process_input, out); break;
  case effect_type::shaper: process_shp(process_input, out); break;
  case effect_type::filter: process_flt(process_input, out, scratch); break;
  case effect_type::reverb: process_reverb(process_input, out, scratch); break;
  default: assert(false); break;
  }
  result.own_time = performance_counter() - start_time;
  return result;
} 

} // namespace inf::synth 