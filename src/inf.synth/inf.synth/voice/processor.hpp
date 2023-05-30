#ifndef INF_SYNTH_VOICE_PROCESSOR_HPP
#define INF_SYNTH_VOICE_PROCESSOR_HPP

#include <inf.synth/amp/processor.hpp>
#include <inf.synth/lfo/processor.hpp>
#include <inf.synth/effect/processor.hpp>
#include <inf.synth/cv_bank/processor.hpp>
#include <inf.synth/envelope/processor.hpp>
#include <inf.synth/oscillator/processor.hpp>
#include <inf.synth/audio_bank/processor.hpp>

#include <array>
#include <cstdint>

namespace inf::synth {

struct cpu_usage;
struct voice_input;
struct scratch_space;

// Single voice in polyphonic synth.
class voice_processor
{
  float _sample_rate;
  std::int32_t _midi;
  std::int32_t _last_midi; // For portamento in poly mode.
  bool _new_voice_section; // If this voice raised voice count to 1.

  // For portamento.
  bool _voice_start;
  double _port_acc;
  double _port_current;
  double _port_target;
  std::int32_t _port_pos;
  std::int32_t _port_samples;

  scratch_space* _scratch;
  cv_bank_state* _cv_state;
  audio_bank_state* _audio_state;
  std::vector<float>* _port_state;
  base::topology_info const* _topology;

  amp_bal_processor _vamp_bal;
  cv_bank_processor _vcv_bank;
  audio_bank_processor _vaudio_bank;

  std::array<lfo_processor, vlfo_count> _vlfos;
  std::array<envelope_processor, venv_count> _venvs;
  std::array<oscillator_processor, vosc_count> _voscs;
  std::array<effect_processor, veffect_count> _veffects;

  // This CV is fixed at voice start.
  float _velo;
  std::array<cv_hold_sample, glfo_count> _glfo_hold;
  std::array<cv_hold_sample, master_gcv_count> _gcv_bi_hold;
  std::array<cv_hold_sample, master_gcv_count> _gcv_uni_hold;

  // Portamento control.
  void prepare_port(voice_input const& input);
  void init_port(std::int32_t mode, std::int32_t samples, std::int32_t prev_midi);

public:
  // With input and audio offset to the start of the voice within the current 
  // block. Release sample is nonnegative if voice is released within the current block.
  // Automation should be fixed to the last active value if this voice is released,
  // this is handled globally by the synth class. Returns true if voice ended.
  // In mono/legato mode, switching to new midi note at new midi pos, taking into account portamento settings.
  bool process(voice_input const& input, cpu_usage& usage);

  voice_processor() = default;
  voice_processor(base::topology_info const* topology, float sample_rate,
    oscillator_state* oscillator_state, effect_state* fx_state, 
    audio_bank_state* audio_state, cv_bank_state* cv_state,
    cv_hold_sample const* gcv_uni_hold_, cv_hold_sample const* gcv_bi_hold_, 
    cv_hold_sample const* glfo_hold_, float velo, std::vector<float>* port_state, scratch_space* scratch, 
    std::int32_t midi, std::int32_t last_midi, bool new_voice_section, base::block_input_data const& input);
};

} // namespace inf::synth
#endif // INF_SYNTH_VOICE_PROCESSOR_HPP
