#include <inf.synth/voice/state.hpp>
#include <inf.synth/output/state.hpp>
#include <inf.synth/voice/processor.hpp>
#include <inf.base/shared/support.hpp>

#include <cassert>
#include <algorithm>

using namespace inf::base;

namespace inf::synth {

voice_processor::
voice_processor(topology_info const* topology, float sample_rate,
  oscillator_state* oscillator_state, effect_state* fx_state,
  audio_bank_state* audio_state, cv_bank_state* cv_state,
  cv_hold_sample const* gcv_hold_, cv_hold_sample const* glfo_hold_, float velo,
  std::vector<float>* port_state, scratch_space* scratch, std::int32_t midi,
  std::int32_t last_midi, bool new_voice_section, block_input_data const& input):
_sample_rate(sample_rate), _midi(midi), _last_midi(last_midi), _new_voice_section(new_voice_section),
_voice_start(true), _port_acc(0.0f), _port_current(0.0f), _port_target(0.0), _port_pos(0), _port_samples(-1),
_scratch(scratch), _cv_state(cv_state), _audio_state(audio_state), _port_state(port_state), _topology(topology),
_vamp_bal(topology, part_type::voice, sample_rate), _vcv_bank(topology, cv_state, gcv_hold_, glfo_hold_, velo, midi, input), 
_vaudio_bank(input, audio_state), _vlfos(), _venvs(), _voscs(), _veffects(), _velo(velo), _glfo_hold(), _gcv_hold()
{  
  assert(topology != nullptr);
  assert(0 <= midi && midi < 128);
  assert(0.0f <= velo && velo <= 1.0f);
  std::copy(glfo_hold_, glfo_hold_ + glfo_count, _glfo_hold.begin());
  std::copy(gcv_hold_, gcv_hold_ + master_gcv_count, _gcv_hold.begin());

  part_id voice_id = { part_type::voice, 0 };
  automation_view automation = input.automation.rearrange_params(voice_id);
  std::int32_t voice_oct = automation.block_discrete(voice_param::oct);
  std::int32_t voice_note = automation.block_discrete(voice_param::note);
  std::int32_t voice_midi_offset = 12 * (voice_oct + 1) + voice_note - midi_note_c4;
  assert(-128 <= voice_midi_offset && voice_midi_offset < 128);

  // Use midi_offset for osc, so we can offset all oscillators at once.
  // Don't use it for filter fx, since we don't keyboard track the
  // master section midi offset, only the actual incoming midi notes.
  for (std::int32_t i = 0; i < venv_count; i++)
    _venvs[i] = envelope_processor(topology, i, sample_rate, input.bpm, input.automation);
  for (std::int32_t i = 0; i < vlfo_count; i++)
    _vlfos[i] = lfo_processor(topology, { part_type::vlfo, i }, sample_rate, input.bpm, input.automation);
  for (std::int32_t i = 0; i < vosc_count; i++)
    _voscs[i] = oscillator_processor(topology, i, sample_rate, input, midi, voice_midi_offset, &oscillator_state[i]);
  for (std::int32_t i = 0; i < veffect_count; i++)
    _veffects[i] = effect_processor(topology, { part_type::veffect, i }, sample_rate, input.bpm, midi, &fx_state[i], input.automation);
}

void voice_processor::prepare_port(voice_input const& input)
{
  std::int32_t port_mode = input.port_mode;
  std::int32_t port_trig = input.port_trig;
  std::int32_t port_samples = input.port_samples;
  std::int32_t master_mode = input.master_mode;
  std::int32_t sample_count = input.block->sample_count;
  std::int32_t new_midi = input.new_midi;
  std::int32_t new_midi_start_pos = input.new_midi_start_pos;

  // Set portamento state.
  std::fill(_port_state->begin(), _port_state->begin() + sample_count, 1.0f);
  for (std::int32_t p = 0; p < sample_count; p++)
  {
    // Poly mode portamento kickin in at the first sample.
    if (_voice_start &&
      master_mode == master_mode::poly && (
        port_trig == master_port_trig::note ||
        (port_trig == master_port_trig::voice && !_new_voice_section)))
      init_port(port_mode, port_samples, _last_midi);

    // Mono mode portamento kickin in at voice or note start.
    if (p == new_midi_start_pos &&
      master_mode != master_mode::poly && (
        port_trig == master_port_trig::note ||
        (port_trig == master_port_trig::voice && !_voice_start)))
    { 
      std::int32_t prev_midi = _last_midi;
      if (!_voice_start)
      {
        prev_midi = _midi;
        _midi = new_midi;
      }
      init_port(port_mode, port_samples, prev_midi);
    }
    _voice_start = false;

    // Init_port may have reset stuff, so check must be re-evaluated. 
    if (_port_pos < _port_samples)
    {
      (*_port_state)[p] = static_cast<float>(_port_current / _port_target);
      _port_current *= _port_acc;
      _port_pos++;
    }
  }

  // Validate portamento state.
  for (std::int32_t p = 0; p < sample_count; p++)
    assert((*_port_state)[p] > 0.0f);
}

// Set portamento params.
void voice_processor::init_port(std::int32_t mode, std::int32_t samples, std::int32_t prev_midi)
{
  _port_pos = 0;
  assert(0 <= prev_midi && prev_midi < 128);

  // No glide.
  if (mode == master_port_mode::off)
  {
    _port_samples = 0;
    _port_acc = 1.0;
    _port_target = 1.0;
    _port_current = 1.0;
    return;
  }

  // Glide from previous note in the direction of this note.
  float fthis_midi = static_cast<float>(_midi);
  float fprev_midi = static_cast<float>(prev_midi);
  float range_semis = fthis_midi - fprev_midi;
  float range_octs = range_semis / 12.0f;
  float range_relative = std::pow(2.0f, range_octs);
  _port_target = range_relative;

  // Auto covers exactly the pitch difference between previous and current note in exactly N samples.
  if (mode == master_port_mode::auto_)
    _port_samples = samples;
  // Manual allows the user to specify how much time to cover 1 semitone, so total glide length is variable.
  else
    _port_samples = static_cast<std::int32_t>(samples * std::abs(range_semis));

  _port_acc = std::pow(10.0, 1.0 / _port_samples * std::log10(range_relative));
  _port_current = _port_acc;
  assert(_port_current > 0.0f);
  assert(_port_target > 0.0f);
  assert(_port_samples >= 0);
}

bool voice_processor::process(voice_input const& input, cpu_usage& usage)
{
  audio_bank_output audio_mixdown;
  std::int32_t master_mode = input.master_mode;
  std::int32_t release_sample = input.release_sample;
  std::int32_t sample_count = input.block->sample_count;
  std::int32_t new_midi = input.new_midi;
  std::int32_t new_midi_start_pos = input.new_midi_start_pos;

  // If released this buffer, 0 <= release_sample < sample_count.
  assert(0 <= release_sample && release_sample <= sample_count);

  // Set portamento state.
  prepare_port(input);

  // Apply voice-static cv (velocity, hold master cv, hold lfos).
  _vcv_bank.apply_voice_state(_gcv_hold.data(), _glfo_hold.data(), _velo, _midi, sample_count);

  // Run lfo's.
  for (std::int32_t i = 0; i < vlfo_count; i++)
    usage.vlfo += _vlfos[i].process_any(*input.block, _cv_state->vlfo[i].buffer, *_scratch);

  // Run envelopes.
  bool ended = false;
  std::int32_t retrig_pos = -1;
  if (master_mode != master_mode::poly) retrig_pos = new_midi_start_pos;
  for (std::int32_t i = 0; i < venv_count; i++)
  {
    envelope_input env_in;
    env_in.offset = 0;
    env_in.block = input.block;
    env_in.retrig_pos = retrig_pos;
    env_in.release_sample = release_sample;
    envelope_output envelenv_out = _venvs[i].process(env_in, _cv_state->venv[i].buffer);
    usage.env += envelenv_out.own_time;
    if(i == 0) ended = envelenv_out.ended;
  }

  // Clear output in case user selected weird RM/AM/Sync routing (e.g., osc 2 to 1).
  for (std::int32_t i = 0; i < vosc_count; i++)
  {
    _audio_state->vosc[i].clear(sample_count);
    _audio_state->vosc_reset[i].clear(sample_count);
  }
  
  // Allow access to previous (and self) osc output for RM/AM/Sync.
  float* const* vosc_buffers[vosc_count];
  float* const* vosc_reset_pos_buffers[vosc_count];
  std::int32_t* const* vosc_reset_buffers[vosc_count];
  for(std::int32_t i = 0; i < vosc_count; i++)
  {
    vosc_buffers[i] = _audio_state->vosc[i].buffers();
    vosc_reset_buffers[i] = _audio_state->vosc_reset[i].buffers();
    vosc_reset_pos_buffers[i] = _audio_state->vosc_reset_pos[i].buffers();
  }

  // Run generators.
  for (std::int32_t i = 0; i < vosc_count; i++)
  {
    oscillator_input osc_in;
    osc_in.block = input.block;
    osc_in.portamento = _port_state->data();
    osc_in.ram_in = vosc_buffers;
    osc_in.sync_reset = vosc_reset_buffers;
    osc_in.sync_reset_pos = vosc_reset_pos_buffers;
    osc_in.new_midi = new_midi;
    osc_in.new_midi_start_pos = new_midi_start_pos;
    audio_part_output osc_out = _voscs[i].process(osc_in, _audio_state->vosc[i].buffers(), _vcv_bank, *_scratch);
    usage.vcv += osc_out.cv_time;
    usage.osc += osc_out.own_time;
  }

  // Clear fx output in case user selected weird routing (i.e. fx 3 to fx 2).
  for (std::int32_t i = 0; i < veffect_count; i++)
    _audio_state->veffect[i].clear(sample_count);

  // Run fx.
  for (std::int32_t i = 0; i < veffect_count; i++) 
  {
    part_id veffect_mod_id = { vaudio_route_output::veffect, i };
    audio_mixdown = _vaudio_bank.process(*input.block, veffect_mod_id, _vcv_bank);
    usage.vcv += audio_mixdown.cv_time;
    usage.vaudio += audio_mixdown.own_time;
    effect_input fx_input;
    fx_input.audio_in = audio_mixdown.mixdown;
    fx_input.block = input.block;
    fx_input.new_midi = new_midi;
    fx_input.new_midi_start_pos = new_midi_start_pos;
    audio_part_output fx_output = _veffects[i].process_any(fx_input, _audio_state->veffect[i].buffers(), _vcv_bank, *_scratch);
    usage.vcv += fx_output.cv_time;
    usage.veffect += fx_output.own_time;
  }
   
  // Run amp section.
  unipolar_untransform(_cv_state->venv[0].buffer, sample_count);
  part_id voice_mod_id = { vaudio_route_output::voice, 0 };
  audio_mixdown = _vaudio_bank.process(*input.block, voice_mod_id, _vcv_bank);
  usage.vcv += audio_mixdown.cv_time;
  usage.vaudio += audio_mixdown.own_time;

  amp_bal_input amp_bal_in;
  amp_bal_in.block = input.block;
  amp_bal_in.audio_in = audio_mixdown.mixdown;
  amp_bal_in.amp_env = _cv_state->venv[0].buffer.values;
  audio_part_output amp_bal_out = _vamp_bal.process(amp_bal_in, _audio_state->voice.buffers(), _vcv_bank);
  usage.vcv += amp_bal_out.cv_time;
  usage.voice += amp_bal_out.own_time;
  return ended;
} 
  
} // namespace inf::synth 
