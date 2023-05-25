#ifndef INF_SYNTH_AUDIO_BANK_STATE_HPP
#define INF_SYNTH_AUDIO_BANK_STATE_HPP

#include <inf.base/shared/cont_storage.hpp>

#include <array>
#include <vector>
#include <cstdint>
#include <utility>

namespace inf::synth {

struct audio_bank_output
{
  std::int64_t cv_time;
  std::int64_t own_time;
  float const* const* mixdown;
};

struct audio_route_indices
{
  std::int32_t route_index;
  std::pair<std::int32_t, std::int32_t> input_ids;
};

struct audio_bank_data
{
  static audio_bank_data const voice;
  static audio_bank_data const global;

  std::int32_t const part_type;
  std::int32_t const route_count;
  std::int32_t const modulation_type;
  std::int32_t const total_route_count;

  std::vector<std::vector<std::int32_t>> const input_table_in;
  std::vector<std::vector<std::int32_t>> const output_table_in;
  std::vector<std::pair<std::int32_t, std::int32_t>> const input_table_out;
};

struct audio_bank_state
{
  base::cont_storage<float> out; // Mixdown result.
  base::cont_storage<float> voice; // Voice amp in 1 voice.
  base::cont_storage<float> voices; // Mixdown of all (max: polyphony) voice amps.
  base::cont_storage<float> external; // External input in effect mode.
  base::cont_storage<float> vosc_all; // Mixdown of all oscs in 1 voice.
  std::array<base::cont_storage<float>, vosc_count> vosc; // Individual oscs in 1 voice. 
  std::array<base::cont_storage<float>, veffect_count> veffect; // Individual fx in 1 voice.
  std::array<base::cont_storage<float>, geffect_count> geffect; // Individual global fx.
  std::array<base::cont_storage<float>, vosc_count> vosc_reset_pos; // Osc reset subsample positions of osc_count * unison_count * sample_count for sync.
  std::array<base::cont_storage<std::int32_t>, vosc_count> vosc_reset; // Osc reset occurred 0/1 for sync.
  explicit audio_bank_state(std::int32_t max_sample_count);
};

} // namespace inf::base
#endif // INF_SYNTH_AUDIO_BANK_STATE_HPP