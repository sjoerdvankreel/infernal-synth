#include <inf.synth/cv_bank/processor.hpp>
#include <inf.synth/oscillator/topology.hpp>
#include <inf.synth/oscillator/processor.hpp>
#include <inf.synth/shared/scratch_space.hpp>
#include <inf.synth/shared/basic_lp_filter.hpp>
#include <inf.synth/oscillator/processor_dsf.hpp>
#include <inf.synth/oscillator/processor_kps.hpp>
#include <inf.synth/oscillator/processor_mix.hpp>
#include <inf.synth/oscillator/processor_noise.hpp>
#include <inf.synth/oscillator/processor_basic.hpp>

using namespace inf::base;

namespace inf::synth {

// 0 = 100% left, 1 = 100% right, sqrt taper, -3dB at center.
// https://www.kvraudio.com/forum/viewtopic.php?t=148865.
static inline float
mono_pan_sqrt_3(std::int32_t channel, float panning)
{
  assert(channel == 0 || channel == 1);
  if (channel == 1) return std::sqrt(panning);
  return std::sqrt(1.0f - panning);
}

oscillator_processor::
oscillator_processor(
topology_info const* topology, std::int32_t index, float sample_rate,
block_input_data const& input, std::int32_t midi, std::int32_t midi_offset, oscillator_state* state):
audio_part_processor(topology, { part_type::vosc, index }, sample_rate, vcv_route_output::vosc),
_on(), _type(), _am_src(), _sync_src(), _kbd_track(0), _dsf_parts(), _basic_type(), 
_uni_voices(), _midi_note(), _state(state), _midi_offset(midi_offset)
{
  assert(state != nullptr);
  update_midi_kbd(input, midi);

  automation_view automation(input.automation.rearrange_params({ part_type::vosc, index }));
  std::int32_t noise_seed = automation.block_discrete(osc_param::noise_seed);
  _on = automation.block_discrete(osc_param::on);
  _type = automation.block_discrete(osc_param::type);
  _am_src = automation.block_discrete(osc_param::am_src);
  _sync_src = automation.block_discrete(osc_param::sync_src);
  _dsf_parts = automation.block_discrete(osc_param::dsf_parts);
  _basic_type = automation.block_discrete(osc_param::basic_type);
  _uni_voices = automation.block_discrete(osc_param::uni_voices);

  // Clear out sync state.
  for (std::int32_t v = 0; v < _uni_voices; v++)
  {
    _state->unsynced_phases[v] = 0.0f;
    _state->sync_unprocessed_samples[v] = 0;
  }

  // Phases for memoryless generators.
  float uni_offset = automation.block_real_transform(osc_param::uni_offset);
  float uni_offset_apply = _uni_voices == 1? 0.0f: uni_offset;
  for (std::int32_t i = 0; i < _uni_voices; i++)
    _state->phases[i] = static_cast<float>(i) / _uni_voices * uni_offset_apply;

  // Clear out noise initial state.
  if (_type == osc_type::noise)
  {
    _state->noise_prev_draw_phase = 1.0f;
    _state->noise_rand_state_x = std::numeric_limits<uint32_t>::max() / noise_seed;
    _state->noise_rand_state_y = std::numeric_limits<uint32_t>::max() / noise_seed;
    _state->noise_prev_draw = fast_rand_next(_state->noise_rand_state_x);
    std::fill(state->noise_color_hold.begin(), state->noise_color_hold.end(), 0);
    std::fill(state->noise_color_value.begin(), state->noise_color_value.end(), 0.0f);
  }

  // Clear out kps initial state.
  if(_type == osc_type::kps)
  {
    _state->kps_initialized = false;
    float cent = automation.block_real_transform(osc_param::cent);
    float frequency = note_to_frequency_table(static_cast<float>(midi + cent));
    std::int32_t kps_length = static_cast<std::int32_t>(sample_rate / frequency);
    for (std::int32_t v = 0; v < _uni_voices; v++)
      _state->kps_positions[v] = static_cast<std::int32_t>(_state->phases[v] * kps_length);
  }
}

// Set up per-voice unison bursts for KPS.
void 
oscillator_processor::init_kps(float filter_amt)
{
  // Initial white noise + filter amount.
  basic_lp_filter filter = {};
  std::uint32_t rand_state = 1;
  filter.init(sample_rate(), 20.0f + (sample_rate() * 0.5f - 20.0f) * filter_amt);
  for (std::int32_t i = 0; i < _state->max_kps_length; i++)
  {
    float sample = filter.next(fast_rand_next(rand_state));
    for (std::int32_t v = 0; v < _uni_voices; v++)
      _state->kps_lines[v][i] = sample * 2.0f - 1.0f;
  }
}

// In mono/legato mode, we need to update base freq of a running voice.
void
oscillator_processor::update_midi_kbd(block_input_data const& input, std::int32_t midi)
{
  automation_view automation(input.automation.rearrange_params(id()));
  std::int32_t oct = automation.block_discrete(osc_param::oct);
  std::int32_t note = automation.block_discrete(osc_param::note);
  _kbd_track = automation.block_discrete(osc_param::kbd);
  _midi_note = _midi_offset + 12.0f * (oct + 1.0f) + note + (_kbd_track ? midi : midi_note_c4) - midi_note_c4;
}

template <class processor_type>
void 
oscillator_processor::process(oscillator_input const& input, 
  float const* const* params, float* const* out, scratch_space& scratch, processor_type processor)
{
  float const min_fm = 20.0f;
  float const max_fm = 10000.0f;

  // These are not phase based. Sync could probably be made to work for kps
  // by resetting the table somehow, but doesn't seem like a real use case.
  bool apply_sync = _type != osc_type::kps && _type != osc_type::noise;

  float const* pm = params[osc_param::pm];
  float const* fm = params[osc_param::fm];
  float const* cent = params[osc_param::cent];
  float const* detune = params[osc_param::uni_dtn];
  float const* spread = params[osc_param::uni_sprd];
  float const* am_mix = params[osc_param::am_mix];
  float const* am_ring = params[osc_param::am_ring];

  float voice_count = static_cast<float>(_uni_voices);
  float voice_apply = _uni_voices == 1? 0.0f: 1.0f;
  float voice_range = _uni_voices == 1 ? 1.0f : static_cast<float>(_uni_voices - 1);

  // Precompute voice independent stuff (only sample dependent).
  float* pan_min = scratch.storage_f32.buffer(0);
  float* pan_max = scratch.storage_f32.buffer(1);
  float* midi_min = scratch.storage_f32.buffer(2); 
  float* midi_max = scratch.storage_f32.buffer(3);
  float* base_freq = scratch.storage_f32.buffer(4);
  float* portamento = scratch.storage_f32.buffer(5);
  if(_kbd_track == 0) std::fill(portamento, portamento + input.block->sample_count, 1.0f);
  else std::copy(input.portamento, input.portamento + input.block->sample_count, portamento);

  for (std::int32_t s = 0; s < input.block->sample_count; s++)
  {
    // Midi switching for mono mode.
    if (s == input.new_midi_start_pos) update_midi_kbd(*input.block, input.new_midi);
    float spread_apply = spread[s] * voice_apply * 0.5f;
    pan_min[s] = 0.5f - spread_apply;
    pan_max[s] = 0.5f + spread_apply;
    float midi_cent = _midi_note + cent[s];
    float detune_apply = detune[s] * voice_apply * 0.5f;
    midi_min[s] = midi_cent - detune_apply;
    midi_max[s] = midi_cent + detune_apply;
    base_freq[s] = note_to_frequency_table(midi_cent) * portamento[s];
    assert(base_freq[s] > 0.0f);

    // https://www.musicdsp.org/en/latest/Synthesis/160-phase-modulation-vs-frequency-modulation-ii.html
    if (fm[s] > 0) base_freq[s] += std::clamp((max_fm - base_freq[s]), base_freq[s], max_fm) * fm[s];
    if (fm[s] < 0) base_freq[s] += std::clamp((base_freq[s] - min_fm), min_fm, base_freq[s]) * fm[s];
  }

  // Run oscillators in unison.
  for (std::int32_t v = 0; v < _uni_voices; v++)
  {
    float* voice_out = scratch.storage_f32.buffer(6);
    float* voice_pan = scratch.storage_f32.buffer(7);

    for (std::int32_t s = 0; s < input.block->sample_count; s++)
    {
      // Find out spread and detune.
      float voice_midi = midi_min[s] + (midi_max[s] - midi_min[s]) * v / voice_range;
      float voice_freq = note_to_frequency_table(voice_midi) * portamento[s];
      assert(voice_freq > 0.0f);
      // https://www.musicdsp.org/en/latest/Synthesis/160-phase-modulation-vs-frequency-modulation-ii.html
      if (fm[s] > 0) voice_freq += std::clamp((max_fm - voice_freq), voice_freq, max_fm) * fm[s];
      if (fm[s] < 0) voice_freq += std::clamp((voice_freq - min_fm), min_fm, voice_freq) * fm[s];
      float voice_increment = voice_freq / sample_rate();
      voice_pan[s] = pan_min[s] + (pan_max[s] - pan_min[s]) * v / voice_range;

      // Oscillator for this voice, including phasemod and hardsync.

      // We sort-of follow urs's method of crossfading between synced and unsynced,
      // but rather than keeping history of either signal, we generate both on the
      // fly for fade_length samples, after that continuing with the synced version
      // only. Seems easier, because calculating stuff back in time means we also 
      // have to keep track of old modulation values which may cross block boundaries.
      // https://www.kvraudio.com/forum/viewtopic.php?t=192829&highlight=oscillator+sync
      if(apply_sync && _sync_src != osc_sync_off && input.sync_reset[_sync_src - 1][v][s] != 0)
      {
        // Keep track of the unsynced version for fade_length samples.
        _state->unsynced_phases[v] = _state->phases[v];
        _state->sync_unprocessed_samples[v] = osc_sync_fade_length;
        // Reset to where we would have been if not discretely sampled.
        _state->phases[v] = input.sync_reset_pos[_sync_src - 1][v][s] * voice_increment;
      }

      // Crossfading unsynced and synced phase signals for hardsync.
      float sample = processor(v, voice_freq, _state->phases[v], voice_increment, s);
      if (apply_sync && _state->sync_unprocessed_samples[v] > 0)
      {
        float unsynced = processor(v, voice_freq, _state->unsynced_phases[v], voice_increment, s);
        _state->unsynced_phases[v] += pm[s] * voice_freq / base_freq[s] + voice_increment;
        _state->unsynced_phases[v] -= std::floor(_state->unsynced_phases[v]);
        float unsynced_weight = _state->sync_unprocessed_samples[v] / (osc_sync_fade_length + 1.0f);
        _state->sync_unprocessed_samples[v]--;
        sample = unsynced_weight * unsynced + (1.0f - unsynced_weight) * sample;
      }
      voice_out[s] = sample;

      // https://www.musicdsp.org/en/latest/Synthesis/160-phase-modulation-vs-frequency-modulation-ii.html
      _state->phases[v] += pm[s] * voice_freq / base_freq[s] + voice_increment;

      // Bookkeeping in case we are the sync source.
      if(apply_sync && _state->phases[v] >= 1.0f)
      {
        _state->phases[v] -= std::floor(_state->phases[v]);
        // How much subsamples did we overshoot?
        input.sync_reset_pos[id().index][v][s] = _state->phases[v] / voice_increment;
        input.sync_reset[id().index][v][s] = 1;
      }
    } 

    // Apply panning and add scaled voice output to total osc output.
    // https://en.wikipedia.org/wiki/Panning_(audio):
    // "If the two output buses are to remain stereo then a law of -3 dB is desirable."
    for (std::int32_t c = 0; c < stereo_channels; c++)
      for(std::int32_t s = 0; s < input.block->sample_count; s++)
        out[c][s] = sanity(out[c][s] + mono_pan_sqrt_3(c, voice_pan[s]) * voice_out[s] / voice_count);
  }

  // Apply ring mod/amp mod.
  for (std::int32_t c = 0; c < stereo_channels; c++)
    for (std::int32_t s = 0; s < input.block->sample_count; s++)
    {
      float carrier = out[c][s];
      float balance = 1.0f - am_ring[s];
      float modulator = (input.am_in[_am_src][c][s] + balance) / (1.0f + balance);
      out[c][s] = ((1.0f - am_mix[s]) * carrier + am_mix[s] * carrier * modulator);
    }
}

void oscillator_processor::process_dsf(oscillator_input const& input,
  float const* const* params, float* const* out, scratch_space& scratch)
{
  float const* dist_param = params[osc_param::dsf_dist];
  float const* decay_param = params[osc_param::dsf_decay];
  auto processor = osc_dsf_processor({ sample_rate(), _dsf_parts, dist_param, decay_param });
  process(input, params, out, scratch, processor);
}

void oscillator_processor::process_noise(oscillator_input const& input,
  float const* const* params, float* const* out, scratch_space& scratch)
{
  float const* noise_x_param = params[osc_param::noise_x];
  float const* noise_y_param = params[osc_param::noise_y];
  float const* noise_color_param = params[osc_param::noise_color];
  auto processor = osc_noise_processor({ _state, noise_x_param, noise_y_param, noise_color_param });
  process(input, params, out, scratch, processor);
}

void oscillator_processor::process_mix(oscillator_input const& input,
  float const* const* params, float* const* out, scratch_space& scratch)
{
  float const* mix_pw_param = params[osc_param::mix_pw];
  float const* mix_saw_param = params[osc_param::mix_saw];
  float const* mix_sine_param = params[osc_param::mix_sine];
  float const* mix_tri_param = params[osc_param::mix_triangle];
  float const* mix_pulse_param = params[osc_param::mix_pulse];
  auto processor = osc_mix_processor({ mix_sine_param, mix_saw_param, mix_pulse_param, mix_tri_param, mix_pw_param });
  process(input, params, out, scratch, processor);
}

void oscillator_processor::process_kps(oscillator_input const& input,
  float const* const* params, float* const* out, scratch_space& scratch)
{
  float const* filter_param = params[osc_param::kps_filter];
  float const* stretch_param = params[osc_param::kps_stretch];
  float const* feedback_param = params[osc_param::kps_feedback];

  // Filter amount is not really a continuous parameter,
  // but we do it this way so it can participate in modulation.
  // Seems like a nice mod target for velocity.
  if (!_state->kps_initialized)
  {
    init_kps(filter_param[0]);
    _state->kps_initialized = true;
  }
  auto processor = osc_kps_processor({ _state, sample_rate(), stretch_param, feedback_param });
  process(input, params, out, scratch, processor);
}

void oscillator_processor::process_basic(oscillator_input const& input,
  float const* const* params, float* const* out, scratch_space& scratch)
{
  float const* pw_param = params[osc_param::basic_pw];
  switch (_basic_type)
  {
  case osc_basic_type::sine: process(input, params, out, scratch, osc_sine_processor()); break;
  case osc_basic_type::saw: process(input, params, out, scratch, osc_blep_saw_processor()); break;
  case osc_basic_type::triangle: process(input, params, out, scratch, osc_blamp_triangle_processor()); break;
  case osc_basic_type::pulse: process(input, params, out, scratch, osc_blep_pulse_processor({ pw_param })); break;
  default: assert(false); break;
  }
}

audio_part_output
oscillator_processor::process(oscillator_input const& input, 
  float* const* out, cv_bank_processor& cv, scratch_space& scratch)
{    
  // Note, sync_reset and sync_reset_pos should be cleared prior to usage.
  // This is handled by osc_graph and synth_voice.
  // For graph, ram_in is set to 1 so we get nice pictures.
  // This does mean we have to clear output per-osc here.
  // Then, each unison voice will add to it.
  for (std::int32_t c = 0; c < stereo_channels; c++)
    std::memset(out[c], 0, input.block->sample_count * sizeof(float));

  // Disabled, return silence.
  audio_part_output result = { 0, 0 };
  if (_on == 0) return result;

  // Modulation.
  cv_bank_input cv_input;
  float const* const* params;
  automation_view automation(input.block->automation.rearrange_params(id()));
  cv_input.block = input.block;
  cv_input.automated = &automation;
  cv_input.modulation_id = modulation_id();
  result.cv_time = cv.modulate(cv_input, params);

  // Run unison over oscillator.
  std::int64_t start_time = performance_counter();
  switch (_type)
  {
  case osc_type::kps: process_kps(input, params, out, scratch); break;
  case osc_type::dsf: process_dsf(input, params, out, scratch); break;
  case osc_type::mix: process_mix(input, params, out, scratch); break;
  case osc_type::noise: process_noise(input, params, out, scratch); break;
  case osc_type::basic: process_basic(input, params, out, scratch); break;
  default: assert(false); break;
  }
  result.own_time = performance_counter() - start_time;
  return result;
}

} // namespace inf::synth