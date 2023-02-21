#ifndef SVN_SYNTH_OSCILLATOR_PROCESSOR_HPP
#define SVN_SYNTH_OSCILLATOR_PROCESSOR_HPP

#include <svn.base/processor/state.hpp>
#include <svn.synth/oscillator/state.hpp>
#include <svn.synth/shared/audio_part_processor.hpp>

#include <cstdint>

namespace svn::synth {

struct scratch_space;
class cv_bank_processor;

class oscillator_processor:
public audio_part_processor
{
  std::int32_t _on;
  std::int32_t _type;
  std::int32_t _ram_src;
  std::int32_t _sync_src;
  std::int32_t _kbd_track;
  std::int32_t _dsf_parts;
  std::int32_t _basic_type;
  std::int32_t _uni_voices;

  float _midi_note;
  oscillator_state* _state;
  std::int32_t _midi_offset;

  void init_kps(float filter_amt);
  void update_midi_kbd(base::block_input_data const& input, std::int32_t midi);
  template <class processor_type>
  void process(oscillator_input const& input, float const* const* params, 
    float* const* out, scratch_space& scratch, processor_type processor);

public:
  audio_part_output process(oscillator_input const& input, float* const* out, cv_bank_processor& cv, scratch_space& scratch);

  oscillator_processor() = default;
  oscillator_processor(
    base::topology_info const* topology, std::int32_t index, float sample_rate, 
    base::block_input_data const& input, std::int32_t midi, std::int32_t midi_offset, oscillator_state* state);

private:
  void process_dsf(oscillator_input const& input, float const* const* params, float* const* out, scratch_space& scratch);
  void process_kps(oscillator_input const& input, float const* const* params, float* const* out, scratch_space& scratch);
  void process_mix(oscillator_input const& input, float const* const* params, float* const* out, scratch_space& scratch);
  void process_noise(oscillator_input const& input, float const* const* params, float* const* out, scratch_space& scratch);
  void process_basic(oscillator_input const& input, float const* const* params, float* const* out, scratch_space& scratch);
};

} // namespace svn::synth
#endif // SVN_SYNTH_OSCILLATOR_PROCESSOR_HPP
