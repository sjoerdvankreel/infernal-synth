#ifndef INF_PLUGIN_INFERNAL_SYNTH_SYNTH_PROCESSOR_HPP
#define INF_PLUGIN_INFERNAL_SYNTH_SYNTH_PROCESSOR_HPP

#include <inf.plugin.infernal_synth/synth/state.hpp>
#include <inf.plugin.infernal_synth/amp/processor.hpp>
#include <inf.plugin.infernal_synth/lfo/processor.hpp>
#include <inf.plugin.infernal_synth/voice/processor.hpp>
#include <inf.plugin.infernal_synth/effect/processor.hpp>
#include <inf.plugin.infernal_synth/output/processor.hpp>
#include <inf.plugin.infernal_synth/cv_bank/processor.hpp>
#include <inf.plugin.infernal_synth/audio_bank/processor.hpp>
#include <inf.plugin.infernal_synth/shared/scratch_space.hpp>
#include <inf.base/plugin/audio_processor.hpp>

namespace inf::plugin::infernal_synth {

// Note: release = both note off and no longer tracking automation.
// Could be improved by differentiating note off and channel reuse using
// VST3 noteId, but this currently seems unsupported by my host of choice (renoise).
// Unfortunately this also means a note off event for a specific midi note means
// releasing all voices playing that note.
class synth_processor:
public base::audio_processor
{
  // Global parts.
  output_processor _output;
  amp_bal_processor _gamp_bal;
  cv_bank_processor _gcv_bank;
  audio_bank_processor _gaudio_bank;
  std::array<lfo_processor, glfo_count> _glfos;
  std::array<effect_processor, geffect_count> _geffects;

  // Voice management. 
  // If a voice is activated within the current buffer,
  // it is considered taken from the beginning of the buffer.
  // Similarly, if a voice is finished within the current buffer,
  // it is considered taken untill the end of the buffer.
  // Keep track of last activated voice, seems the safer
  // way to do it when user is switching poly/mono modes on the fly.
  bool _voices_drained;
  bool _last_voice_released;
  std::int32_t _voice_mode;
  std::int32_t _last_midi_note;
  std::int32_t _active_voice_count;
  std::int32_t _last_voice_activated;
  std::array<voice_processor, synth_polyphony> _voices;
  std::array<voice_state, synth_polyphony> _voice_states;

  // Scratch space for anyone who wants it.
  scratch_space _scratch;
  // "Envelope" for global amp.
  std::vector<float> _ones;
  // Keeping track of component's cv outputs for internal routing.
  // Keeping track of component's audio outputs for internal routing.
  // For portamento.
  std::vector<float> _port_state;
  // Of size total voice count.
  // Pointers into _automation_fixed_buffer.  
  std::vector<float*> _automation_fixed;
  // Contiguous array of size total voice count * total param count.
  // Skips over discrete params, only used for continuous.
  std::vector<float> _automation_fixed_buffer;
  // Last automation values of previous buffer, in case we need them on sample 0 the next round.
  // Skips over discrete params, only used for continuous.
  std::vector<float> _last_automation_previous_block;

  // These allocate, so set up once here.
  cv_bank_state _cv_state;
  audio_bank_state _audio_state;
  std::array<effect_state, geffect_count> _global_effect_states;
  std::array<std::array<effect_state, veffect_count>, synth_polyphony> _voice_effect_states;
  std::array<std::array<oscillator_state, vosc_count>, synth_polyphony> _voice_oscillator_states;

  void clear_delay_buffers();

public:
  synth_processor(
    base::topology_info const* topology, base::param_value* state,
    std::int32_t* changed, float sample_rate, std::int32_t max_sample_count);

protected:
  void process(base::block_input const& input, base::block_output& output) override;

private:
  // Process actual audio for an active voice.
  void process_voice(voice_setup_input const& input,
    voice_setup_output const& output, std::int32_t slot, cpu_usage& usage);

  // Oldest voice is recycled first.
  // Toggles _voices_drained to indicate whether an earlier 
  // started, not-yet-finished voice needed to be recycled.
  void release_all_voices();
  std::int32_t find_voice_slot();
  void return_voice(std::int32_t voice);
  void setup_voice_release(base::note_input_event const& note);
  void setup_voice(voice_setup_input const& input, base::note_input_event const& note);

  // Handle note on/off events. This will create and release
  // voices as required by poly/mono mode.
  voice_setup_output process_notes_current_block(voice_setup_input const& input);
  void process_note_on(voice_setup_input const& input, base::note_input_event const& note);
  void process_note_off(voice_setup_input const& input, base::note_input_event const& note);
};

} // namespace inf::plugin::infernal_synth
#endif // INF_PLUGIN_INFERNAL_SYNTH_SYNTH_PROCESSOR_HPP