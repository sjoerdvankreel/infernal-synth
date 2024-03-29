#ifndef INF_PLUGIN_INFERNAL_SYNTH_DSP_CV_BANK_STATE_HPP
#define INF_PLUGIN_INFERNAL_SYNTH_DSP_CV_BANK_STATE_HPP

#include <inf.plugin.infernal_synth/synth/config.hpp>
#include <inf.plugin.infernal_synth/shared/state.hpp>
#include <inf.plugin.infernal_synth/amp/topology.hpp>
#include <inf.plugin.infernal_synth/synth/topology.hpp>
#include <inf.base/plugin/state.hpp>
#include <inf.base/shared/cont_storage.hpp>
#include <inf.base/topology/part_descriptor.hpp>

#include <array>
#include <vector>
#include <cstdint>

namespace inf::plugin::infernal_synth {

struct cv_bank_input
{
  base::part_id modulation_id;
  base::block_input_data const* block;
  base::automation_view const* automated;
};

struct cv_route_indices
{
  std::int32_t route_index;
  std::int32_t target_index;
  std::int32_t input_op_index;
  std::pair<std::int32_t, std::int32_t> input_ids;
};

// Cv buffer plus data ownership.
struct cv_bank_storage
{
  cv_buffer buffer;
  std::vector<float> storage;

  void resize(std::int32_t max_sample_count);
  cv_bank_storage(std::int32_t max_sample_count);

  // Interior pointers.
  cv_bank_storage() = default;
  cv_bank_storage(cv_bank_storage&&) = default;
  cv_bank_storage(cv_bank_storage const&) = delete;
  cv_bank_storage& operator=(cv_bank_storage&&) = default;
  cv_bank_storage& operator=(cv_bank_storage const&) = delete;
};

// To differentiate voice/global.
struct cv_bank_data
{
  static cv_bank_data const voice;
  static cv_bank_data const global;

  std::int32_t const part_type;
  std::int32_t const route_output_total_count;
  std::int32_t const* const route_output_mapping;
  std::int32_t const* const route_output_target_counts;
  std::int32_t const* const* const route_output_target_mapping;

  std::vector<std::vector<std::int32_t>> const output_table_in;
  std::vector<std::pair<std::int32_t, std::int32_t>> const output_table_out;
  std::vector<std::vector<std::int32_t>> const source_table_in;
  std::vector<std::pair<std::int32_t, std::int32_t>> const source_table_out;
  std::vector<std::vector<std::vector<std::int32_t>>> const target_table_in;
};

struct cv_bank_state
{
  std::vector<float> amt;
  std::vector<float> scale;
  std::vector<float> offset;
  std::vector<float> in_modified;
  std::vector<std::int32_t> clamped;
  base::cont_storage<float> out;
  
  std::vector<float> key;
  std::vector<float> key_inv;
  std::vector<float> velo;
  std::vector<float> midi_mod_wheel;
  std::vector<float> midi_ch_vol;
  std::vector<float> midi_ch_press;
  std::vector<float> midi_pitch_bend;

  std::array<cv_bank_storage, venv_count> venv;
  std::array<cv_bank_storage, vlfo_count> vlfo;
  std::array<cv_bank_storage, glfo_count> glfo;
  std::array<cv_bank_storage, glfo_count> glfo_hold;
  std::array<cv_bank_storage, master_gcv_count> gcv_bi;
  std::array<cv_bank_storage, master_gcv_count> gcv_uni;
  std::array<cv_bank_storage, master_gcv_count> gcv_bi_hold;
  std::array<cv_bank_storage, master_gcv_count> gcv_uni_hold;

  cv_bank_state(base::topology_info const* topology, std::int32_t max_sample_count);
};

inline cv_bank_storage::
cv_bank_storage(std::int32_t max_sample_count):
buffer(), storage(max_sample_count) 
{ buffer.values = storage.data(); }

inline void
cv_bank_storage::resize(std::int32_t max_sample_count)
{
  storage.resize(max_sample_count);
  buffer.values = storage.data();
}

} // namespace inf::plugin::infernal_synth
#endif // INF_PLUGIN_INFERNAL_SYNTH_DSP_CV_BANK_STATE_HPP