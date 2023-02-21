#ifndef SVN_SYNTH_OSCILLATOR_STATE_HPP
#define SVN_SYNTH_OSCILLATOR_STATE_HPP

#include <svn.base/processor/state.hpp>
#include <svn.synth/oscillator/config.hpp>

#include <cmath>
#include <array>
#include <vector>
#include <cstdint>

namespace svn::synth {

// New midi: switching base pitch in mono/legato mode.
// Ram_in: osc sources for RM/AM. Note that output is part of ram_in.
struct oscillator_input
{
  std::int32_t new_midi;
  std::int32_t new_midi_start_pos;
  base::block_input_data const* block;
  float const* portamento;
  float* const* const* ram_in;
  float* const* const* sync_reset_pos;
  std::int32_t* const* const* sync_reset;
};

struct oscillator_state
{
  // Phase for memoryless generators.
  std::array<float, osc_max_voices> phases;

  // Bookkeeping for sync.
  // We basically treat the new-synced and old-unsynced versions
  // as 2 separate signals, then cross-fade between them. 
  // https://www.kvraudio.com/forum/viewtopic.php?t=192829&sid=723df45e14c1dad6fc5c3656f70aa006&start=15
  std::array<float, osc_max_voices> unsynced_phases;
  std::array<std::int32_t, osc_max_voices> sync_unprocessed_samples;

  // Feedback lines for karplus-strong.
  bool kps_initialized;
  std::int32_t max_kps_length;
  std::array<std::int32_t, osc_max_voices> kps_positions;
  std::array<std::vector<float>, osc_max_voices> kps_lines;

  // Noise state.
  float noise_prev_draw;
  float noise_prev_draw_phase;
  std::uint32_t noise_rand_state_x;
  std::uint32_t noise_rand_state_y;
  std::array<float, osc_noise_color_bin_count> noise_color_value;
  std::array<std::int32_t, osc_noise_color_bin_count> noise_color_hold;

  oscillator_state() = default;
  oscillator_state(float sample_rate);
};

} // namespace svn::synth
#endif // SVN_SYNTH_OSCILLATOR_STATE_HPP
