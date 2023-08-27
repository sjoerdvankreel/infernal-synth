#ifndef INF_PLUGIN_INFERNAL_SYNTH_SYNTH_STATE_HPP
#define INF_PLUGIN_INFERNAL_SYNTH_SYNTH_STATE_HPP

#include <inf.base/plugin/state.hpp>
#include <cstdint>

namespace inf::plugin::infernal_synth {

// Activation data.
struct voice_setup_input
{
  std::int32_t voice_mode;
  std::int32_t port_mode;
  std::int32_t port_trig;
  std::int32_t port_samples;
  base::block_input const* block;
};

// Need to remember these for mono/legato mode.
struct voice_setup_output
{
  std::int32_t buffer_final_midi;
  std::int32_t buffer_final_midi_pos;
};

// Polyphony bookkeeping.
struct voice_state
{
  // True if processing any audio during current buffer.
  bool in_use = false;
  // True if to be released in current buffer.
  bool release_this_buffer = false;
  // True if already released.
  bool released_previous_buffer = false;
  // Find voice from midi note.
  // Note: for mono/legato mode, this is the FIRST pitch played in this voice.
  // Even as oscillator and keyboard tracking pitch are updated during
  // voice lifetime in monophonic mode, this value is not.
  std::int32_t midi = -1;
  // Start position in stream, used for recycling oldest voice when running out.
  std::int64_t start_position_stream = -1;
  // Start position in current buffer, 0 if started in previous buffer.
  std::int32_t start_position_buffer = -1;
  // Used to stop tracking automation after release.
  std::int32_t release_position_buffer = -1;
};

} // namespace inf::plugin::infernal_synth
#endif // INF_PLUGIN_INFERNAL_SYNTH_SYNTH_STATE_HPP