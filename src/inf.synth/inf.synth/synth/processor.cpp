#include <inf.synth/voice/state.hpp>
#include <inf.synth/synth/processor.hpp>
#include <inf.base/shared/support.hpp>

#include <cassert>
#include <algorithm>

using namespace inf::base;

namespace inf::synth {

synth_processor::
synth_processor(topology_info const* topology, param_value* state, 
  std::int32_t* changed, float sample_rate, std::int32_t max_sample_count):
audio_processor(topology, sample_rate, max_sample_count, state, changed),
_output(topology, sample_rate), _gamp_bal(topology, part_type::master, sample_rate), 
_gcv_bank(topology, &_cv_state), _gaudio_bank(&_audio_state), _glfos(), _geffects(),
_voices_drained(false), _last_voice_released(false), _master_mode(-1), _last_midi_note(-1),
_active_voice_count(0), _last_voice_activated(-1), _voices(), _voice_states(), 
_scratch(max_sample_count), _ones(max_sample_count), _port_state(max_sample_count),
_automation_fixed(synth_polyphony), _automation_fixed_buffer(synth_polyphony * topology->input_param_count),
_last_automation_previous_block(topology->input_param_count),
_cv_state(topology, max_sample_count), _audio_state(max_sample_count),
_global_effect_states(), _voice_effect_states(), _voice_oscillator_states()
{
  assert(state != nullptr);
  assert(sample_rate > 0.0f);
  assert(topology != nullptr);
  assert(max_sample_count > 0);

  // Fake envelope for master.
  std::fill(_ones.begin(), _ones.end(), 1.0f);

  // Initial global voice mode.
  automation_view master_automation(topology, state, { part_type::master, 0 });
  _master_mode = master_automation.block_discrete(master_param::mode);

  // Don't need defaults for discrete valued automation, only contiguous.
  // So we end up with crap in discrete-valued parameters, but they're not used.
  topology->init_defaults(reinterpret_cast<param_value*>(_last_automation_previous_block.data()));

  // Init global fx state.
  for (std::int32_t i = 0; i < geffect_count; i++)
    _global_effect_states[i] = effect_state(true, sample_rate, max_sample_count);

  // Init per-voice osc state.
  for (std::int32_t i = 0; i < synth_polyphony; i++)
    for (std::int32_t j = 0; j < vosc_count; j++)
      _voice_oscillator_states[i][j] = oscillator_state(sample_rate);

  // Init per-voice fx state.
  for (std::int32_t i = 0; i < synth_polyphony; i++)
    for (std::int32_t j = 0; j < veffect_count; j++)
      _voice_effect_states[i][j] = effect_state(false, sample_rate, max_sample_count);

  // Init global lfos.
  for(std::int32_t i = 0; i < glfo_count; i++)
  {
    // 120 = default bpm, process() will overwrite.
    part_id glfo_id = { part_type::glfo, i };
    automation_view lfo_automation(topology, state, glfo_id);
    _glfos[i] = lfo_processor(topology, glfo_id, sample_rate, 120.0, lfo_automation);
  }

  // Init global fx.
  for (std::int32_t i = 0; i < geffect_count; i++)
  {
    // 120 = default bpm, process() will overwrite.
    // midi_note_c4 is to effectively disable keyboard tracking for global filters.
    part_id geffect_id = { part_type::geffect, i };
    automation_view geffect_automation(topology, state, geffect_id);
    _geffects[i] = effect_processor(topology, geffect_id, sample_rate, 120.0f, midi_note_c4, &_global_effect_states[i], geffect_automation);
  }

  for(std::int32_t v = 0; v < synth_polyphony; v++)
    _automation_fixed[v] = _automation_fixed_buffer.data() + v * topology->input_param_count;
}

void
synth_processor::set_processing(bool processing)
{
  // Clear delay lines.
  if(processing) return;
  for(std::int32_t i = 0; i < geffect_count; i++)
  {
    _global_effect_states[i].delay_buffer[0].clear();
    _global_effect_states[i].delay_buffer[1].clear();
    for (std::int32_t j = 0; j < effect_reverb_comb_count; j++)
    {
      std::fill(_global_effect_states[i].reverb_comb[0][j].begin(), _global_effect_states[i].reverb_comb[0][j].end(), 0.0f);
      std::fill(_global_effect_states[i].reverb_comb[1][j].begin(), _global_effect_states[i].reverb_comb[1][j].end(), 0.0f);
    }
    for (std::int32_t j = 0; j < effect_reverb_allpass_count; j++)
    {
      std::fill(_global_effect_states[i].reverb_allpass[0][j].begin(), _global_effect_states[i].reverb_allpass[0][j].end(), 0.0f);
      std::fill(_global_effect_states[i].reverb_allpass[1][j].begin(), _global_effect_states[i].reverb_allpass[1][j].end(), 0.0f);
    }
  }
}

// In case of switching master mode.
void 
synth_processor::release_all_voices()
{
  for (std::int32_t v = 0; v < synth_polyphony; v++)
    if (_voice_states[v].in_use)
      return_voice(v);
}

void
synth_processor::return_voice(std::int32_t index)
{
  assert(_voice_states[index].in_use);
  _voice_states[index] = voice_state();
  --_active_voice_count;
  assert(0 <= _active_voice_count && _active_voice_count < synth_polyphony);
  if(_active_voice_count == 0) _last_voice_activated = -1;
}

void 
synth_processor::setup_voice_release(note_event const& note)
{
  for (std::int32_t v = 0; v < synth_polyphony; v++)
    if (_voice_states[v].in_use && 
      _voice_states[v].midi == note.midi &&
      !_voice_states[v].released_previous_buffer)
    {
      _voice_states[v].release_this_buffer = true;
      _voice_states[v].release_position_buffer = note.sample_index;
    }
}

std::int32_t
synth_processor::find_voice_slot()
{
  _voices_drained = true;
  std::int32_t result = -1;
  std::int64_t slot_position = -1;

  // Either find a free slot or recycle oldest one based on start position in stream.
  for (std::int32_t v = 0; v < synth_polyphony; v++)
    if (!_voice_states[v].in_use)
    {
      result = v;
      _voices_drained = false;
      break;
    }
    else if (result == -1 || _voice_states[v].start_position_stream < slot_position)
    {
      result = v;
      slot_position = _voice_states[v].start_position_stream;
    }
  return result;
}

void 
synth_processor::setup_voice(voice_setup_input const& input, base::note_event const& note)
{
  // Bookkeeping.
  std::int32_t slot = find_voice_slot();
  bool new_voice_section = _active_voice_count == 0;
  _last_voice_activated = slot;
  if(!_voices_drained) _active_voice_count++;
  assert(1 <= _active_voice_count && _active_voice_count <= synth_polyphony);

  // Not released this buffer by default.
  _voice_states[slot].in_use = true;
  _voice_states[slot].midi = note.midi;
  _voice_states[slot].release_this_buffer = false;
  _voice_states[slot].release_position_buffer = -1;
  _voice_states[slot].released_previous_buffer = false;
  _voice_states[slot].start_position_buffer = note.sample_index;
  _voice_states[slot].start_position_stream = input.block->data.stream_position + note.sample_index;

  // Remember voice start global lfo/gcv values for hold-type modulation.
  cv_hold_sample glfo_hold[glfo_count] = { };
  cv_hold_sample gcv_hold[master_gcv_count] = { };
  for(std::int32_t i = 0; i < glfo_count; i++)
  {
    glfo_hold[i].flags = _cv_state.glfo[i].buffer.flags;
    glfo_hold[i].value = _cv_state.glfo[i].buffer.values[note.sample_index];
  } 
  for (std::int32_t i = 0; i < master_gcv_count; i++)
  {
    gcv_hold[i].flags = _cv_state.gcv[i].buffer.flags;
    gcv_hold[i].value = _cv_state.gcv[i].buffer.values[note.sample_index];
  }

  std::int32_t last_midi = _last_midi_note == -1 ? note.midi : _last_midi_note;
  _voices[slot] = voice_processor(topology(), sample_rate(), 
    &_voice_oscillator_states[slot][0], &_voice_effect_states[slot][0],
    &_audio_state, &_cv_state, gcv_hold, glfo_hold, note.velocity, &_port_state,
    &_scratch, note.midi, last_midi, new_voice_section, input.block->data);
} 

void 
synth_processor::process_note_on(voice_setup_input const& input, base::note_event const& note)
{
  // For mono and release, we need to reinitialize portamento settings at the exact sample position. 
  // Regular polyphonic mode: activate new voice for each note.
  // Release monophonic: only if previous voice released, we start a new one.
  // True monophonic: only if there are no active voices (i.e. amp env ended), we start a new one.
  bool new_voice = _last_voice_activated == -1;
  switch (input.master_mode)
  {
  case master_mode::poly: setup_voice(input, note); break;
  case master_mode::mono: if (new_voice) setup_voice(input, note); break;
  case master_mode::release: if (new_voice || _last_voice_released) setup_voice(input, note); break;
  default: assert(false); break;
  }

  // This stuff needs to be AFTER voice activation.
  _last_midi_note = note.midi; // For global filter kbd track and portamento.
  _last_voice_released = false; // For release-trigs-new-voice mode.
}

void 
synth_processor::process_note_off(voice_setup_input const& input, base::note_event const& note)
{
  // For mono and legato, we don't want to release the
  // voice if the note-off event is implicit (i.e.,
  // kicking in by the same channel playing another pitch).
  // Best we can do is check if there's a note on event
  // at the exact same sample position, then assume we need
  // to change pitch. Only if there's a note off event
  // not accompanied by a note-on event at the exact same 
  // position, we release the voice. Note that we need to
  // release the last active voice, because we cannot
  // release "by pitch", since in mono mode that pitch value
  // always holds the initial pitch that activated the voice.
  // Also for poly mode, we need this value for release-reset portamento.
  bool implicit_release = false;
  for (std::int32_t i = 0; i < input.block->note_count; i++)
    if (input.block->notes[i].note_on && input.block->notes[i].sample_index == note.sample_index)
    {
      implicit_release = true;
      break;
    }

  // Need to remember this for release-trigs-new-voice mode.
  _last_voice_released |= !implicit_release;

  // Regular polyphonic mode: release note by pitch.
  // Monophonic modes: release on explicit note-off only.
  note_event event = note;
  if(input.master_mode == master_mode::poly)
    setup_voice_release(event);
  else if(_last_voice_activated != -1 && !implicit_release)
  {
    event.midi = _voice_states[_last_voice_activated].midi;
    setup_voice_release(event);
  }
}

// Release and take new voices as necessary for incoming note events.
voice_setup_output
synth_processor::process_notes_current_block(voice_setup_input const& input)
{
  // Correct voice info for leftovers from previous round.
  for (std::int32_t v = 0; v < synth_polyphony; v++)
    if (!_voice_states[v].in_use)
      _voices_drained = false;
    else
    {
      // Run along from the beginning.
      _voice_states[v].start_position_buffer = 0;
      _voice_states[v].release_this_buffer = false;
    }

  // When switching master voice mode, drop all voices, otherwise we might miss the event that releases 
  // them. Also, when switching per-voice parameters (i.e., those initialized only once at voice start),
  // and we were/are in monophonic mode, also drop all voices. This stuff should never be automated in mono 
  // mode, and it greatly improves the ui experience when manually editing the params of a running mono 
  // voice. (I.e., you don't have to stop/restart the track to get changes picked up).
  bool should_release_all = input.master_mode != _master_mode;
  bool is_or_was_mono = input.master_mode != master_mode::poly || _master_mode != master_mode::poly;
  if (!should_release_all && is_or_was_mono)
    for (std::int32_t i = 0; i < topology()->input_param_count; i++)
      if (topology()->params[i].descriptor->data.kind == param_kind::voice && 
        input.block->changed_automation_raw[i] != 0)
      {
        should_release_all = true;
        break;
      }

  _master_mode = input.master_mode;

  voice_setup_output result;
  result.buffer_final_midi = -1;
  result.buffer_final_midi_pos = -1;
  if (should_release_all)
    release_all_voices();
  else for (std::int32_t n = 0; n < input.block->note_count; n++)
    if (input.block->notes[n].note_on)
    {
      // Need to remember these for mono/legato mode.
      if (input.master_mode != master_mode::poly)
      {
        result.buffer_final_midi = input.block->notes[n].midi;
        result.buffer_final_midi_pos = input.block->notes[n].sample_index;
      }
      process_note_on(input, input.block->notes[n]);
    }
    else
      process_note_off(input, input.block->notes[n]);
  return result;
}

void 
synth_processor::process_voice(voice_setup_input const& input,
  voice_setup_output const& output, std::int32_t slot, cpu_usage& usage)
{
  block_input_data vinput;
  std::int32_t voice_start = _voice_states[slot].start_position_buffer;
  vinput.sample_count = input.block->data.sample_count - voice_start;
  vinput.stream_position = input.block->data.stream_position + voice_start;
  vinput.automation = automation_view(
    topology(), input.block->block_automation_raw,
    input.block->continuous_automation_raw, _automation_fixed[slot],
    topology()->input_param_count, topology()->input_param_count, 0,
    input.block->data.sample_count, voice_start, vinput.sample_count);

  std::int32_t release_sample = vinput.sample_count;
  std::int32_t voice_release = _voice_states[slot].release_position_buffer;

  // Already released in previous buffer, fix from the beginning.
  if (_voice_states[slot].released_previous_buffer)
    vinput.automation = vinput.automation.rearrange_samples(0, 0);

  // Releasing this buffer, do the bookkeeping.
  else if (_voice_states[slot].release_this_buffer)
  {
    // Fix voice to automation values at T-1 if released at time T.
    // This fixes the case where a new note plays in the current channel
    // together with an automation value, in case we want to apply
    // the automation change only to the new note.
    for (std::int32_t p = 0; p < topology()->input_param_count; p++)
      if (topology()->params[p].descriptor->data.is_continuous())
      {
        if (voice_release == 0)
          _automation_fixed[slot][p] = _last_automation_previous_block[p];
        else
          _automation_fixed[slot][p] = input.block->continuous_automation_raw[p][voice_release - 1];
      }

    release_sample = voice_release - voice_start;
    assert(release_sample >= 0);
    vinput.automation = vinput.automation.rearrange_samples(voice_start, release_sample);
    _voice_states[slot].released_previous_buffer = true;
  }

  // Else nothing to do, we ride along with the active automation values.

  // Process voice and mix into combined voices buffer.
  // For mono and legato mode, we now need to switch the active voice
  // to the new note, but that can only be done within the sample loop.
  // So, if relevant, pass the new note index and sample position to
  // the voice, so it can handle mono/legato mode portamento.
  // Note: only if this is the last active voice!
  // Otherwise we'd be retriggering envelopes on mono sections
  // that have already entered the release phase.
  voice_input voice_in;
  voice_in.block = &vinput;
  voice_in.new_midi_start_pos = -1;
  voice_in.port_mode = input.port_mode;
  voice_in.port_trig = input.port_trig;
  voice_in.master_mode = input.master_mode;
  voice_in.port_samples = input.port_samples;
  voice_in.new_midi = output.buffer_final_midi;
  voice_in.release_sample = release_sample;
  if (slot == _last_voice_activated) voice_in.new_midi_start_pos = output.buffer_final_midi_pos - voice_start;
  bool ended = _voices[slot].process(voice_in, usage);
  add_audio(_audio_state.voices.buffers(), _audio_state.voice.buffers(), stereo_channels, vinput.sample_count, voice_start);
  if (ended) return_voice(slot);
}

void 
synth_processor::process(block_input const& input, block_output& output)
{
  cpu_usage usage = { };
  block_input_data vinput;
  vinput.bpm = input.data.bpm;
  audio_bank_output audio_mixdown;
  std::int64_t start_time = performance_counter();

  // Setup block parameters for global banks.
  _gcv_bank.update_block_params(input.data);
  _gaudio_bank.update_block_params(input.data);

  // Clear voice mixdown.
  _audio_state.voices.clear(input.data.sample_count);

  // Run global lfos. 
  for (std::int32_t i = 0; i < glfo_count; i++)
    usage.glfo += _glfos[i].process_global(input.data, _cv_state.glfo[i].buffer, _scratch);

  // Copy external input (zeros in instrument mode).
  for (std::int32_t c = 0; c < stereo_channels; c++)
    std::copy(input.data.audio[c], input.data.audio[c] + input.data.sample_count, _audio_state.external.buffer(c));

  // Set master CV to input buffers.
  part_id master_id = { part_type::master, 0 };
  auto master_automation = input.data.automation.rearrange_params(master_id);
  for(std::int32_t i = 0; i < master_gcv_count; i++)
  { 
    _cv_state.gcv[i].buffer.flags.inverted = false;
    _cv_state.gcv[i].buffer.flags.bipolar = master_automation.block_discrete(master_param::gcv1 + i * 2 + 1) != 0;
    master_automation.continuous_real_transform(master_param::gcv1 + i * 2, _cv_state.gcv[i].buffer.values, input.data.sample_count);
    if(_cv_state.gcv[i].buffer.flags.bipolar)
      unipolar_transform(_cv_state.gcv[i].buffer, input.data.sample_count);
  }

  // Get per-block poly/mono and portamento settings.
  voice_setup_input setup_input;
  setup_input.block = &input;
  setup_input.master_mode = master_automation.block_discrete(master_param::mode);
  setup_input.port_mode = master_automation.block_discrete(master_param::port_mode);
  setup_input.port_trig = master_automation.block_discrete(master_param::port_trig);
  float port_seconds = master_automation.block_real_transform(master_param::port_time);
  std::int32_t port_sync = master_automation.block_discrete(master_param::port_sync);
  std::int32_t port_tempo = master_automation.block_discrete(master_param::port_tempo);
  float port_timesig = master_port_timesig_values[port_tempo];
  if (port_sync == 0) setup_input.port_samples = static_cast<std::int32_t>(sample_rate() * port_seconds);
  else setup_input.port_samples = static_cast<std::int32_t>(timesig_to_samples(sample_rate(), input.data.bpm, port_timesig));

  // Handle note on/off events, this will arrange the per-voice state as necessary.
  voice_setup_output setup_output = process_notes_current_block(setup_input);

  // Clear output.
  for (std::int32_t c = 0; c < stereo_channels; c++)
    std::memset(output.audio[c], 0, input.data.sample_count * sizeof(float));

  // Mix in voices that are active anywhere in this buffer.
  std::int32_t voice_count = 0;
  for(std::int32_t v = 0; v < synth_polyphony; v++)
    if (_voice_states[v].in_use)
    {
      process_voice(setup_input, setup_output, v, usage);
      voice_count++;
    }

  // Clear fx output in case user selected weird routing (i.e. fx 3 to fx 2).
  for (std::int32_t i = 0; i < geffect_count; i++)
    _audio_state.geffect[i].clear(input.data.sample_count);

  // Run fx.
  std::int32_t last_midi = _last_midi_note == -1? midi_note_c4: _last_midi_note;
  for (std::int32_t i = 0; i < geffect_count; i++) 
  {
    part_id geffect_mod_id = { gaudio_route_output::geffect, i };
    audio_mixdown = _gaudio_bank.process(input.data, geffect_mod_id, _gcv_bank);
    usage.gcv += audio_mixdown.cv_time;
    usage.gaudio += audio_mixdown.own_time;
    effect_input fx_input;
    fx_input.block = &input.data;
    fx_input.new_midi = last_midi;
    fx_input.new_midi_start_pos = 0;
    fx_input.audio_in = audio_mixdown.mixdown;
    audio_part_output fx_output = _geffects[i].process_global(fx_input, _audio_state.geffect[i].buffers(), _gcv_bank, _scratch);
    usage.gcv += fx_output.cv_time;
    usage.geffect += fx_output.own_time;
  }
   
  // Run master section.
  part_id master_mod_id = { gaudio_route_output::master, 0 };
  audio_mixdown = _gaudio_bank.process(input.data, master_mod_id, _gcv_bank);
  usage.gcv += audio_mixdown.cv_time;
  usage.gaudio += audio_mixdown.own_time;
  amp_bal_input amp_bal_in;
  amp_bal_in.block = &input.data;
  amp_bal_in.amp_env = _ones.data();
  amp_bal_in.audio_in = audio_mixdown.mixdown;
  audio_part_output amp_bal_out = _gamp_bal.process(amp_bal_in, output.audio, _gcv_bank);
  usage.gcv += amp_bal_out.cv_time;
  usage.master += amp_bal_out.own_time;
  
  // Output round info.
  output_info info;
  info.usage = usage; 
  info.start_time = start_time;
  info.voice_count = voice_count;
  info.drained = _voices_drained;
  info.clipped = audio_may_clip(output.audio, input.channel_count, input.data.sample_count);
  _output.process(input, info, output);

  // Remember last automation values in case a note is released on the 
  // next round at sample index 0. Because if a voice is released at
  // time T then we fix it to the automation values at time T-1.
  for (std::int32_t p = 0; p < topology()->input_param_count; p++)
    if (topology()->params[p].descriptor->data.kind == param_kind::continuous)
      _last_automation_previous_block[p] = input.continuous_automation_raw[p][input.data.sample_count - 1];
}
  
} // namespace inf::synth