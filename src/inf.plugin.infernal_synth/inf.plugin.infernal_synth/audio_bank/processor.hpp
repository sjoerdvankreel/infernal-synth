#ifndef INF_PLUGIN_INFERNAL_SYNTH_AUDIO_BANK_PROCESSOR_HPP
#define INF_PLUGIN_INFERNAL_SYNTH_AUDIO_BANK_PROCESSOR_HPP

#include <inf.base/plugin/state.hpp>
#include <inf.plugin.infernal_synth/audio_bank/state.hpp>
#include <inf.plugin.infernal_synth/audio_bank/topology.hpp>

namespace inf::plugin::infernal_synth {

class cv_bank_processor;

// Combined audio routing, either voice or global.
class audio_bank_processor
{
public:
  audio_bank_processor() = default;
  explicit audio_bank_processor(audio_bank_state* state); // Global
  audio_bank_processor(base::block_input_data const& input, audio_bank_state* state); // Voice

  void update_block_params(base::block_input_data const& input);
  audio_bank_output process(base::block_input_data const& input, base::part_id id, cv_bank_processor& cv);

private:
  audio_bank_state* _state;
  audio_bank_data const* _data = nullptr;

  float const* const* input_buffer(std::int32_t input, std::int32_t index) const;
  float const* const* input_buffer_voice(std::int32_t input, std::int32_t index) const;
  float const* const* input_buffer_global(std::int32_t input, std::int32_t index) const;

  // Precomputing all relevant routes for each target output.
  static inline std::int32_t constexpr max_total_output_count = 
  std::max(vaudio_route_output_total_count, gaudio_route_output_total_count);

  std::array<std::int32_t, max_total_output_count> _relevant_indices_count;
  std::array<std::array<audio_route_indices, audio_bank_route_count>, max_total_output_count> _relevant_indices;
};

inline audio_bank_processor::
audio_bank_processor::audio_bank_processor(audio_bank_state* state):
_state(state), _data(&audio_bank_data::global), _relevant_indices_count(), _relevant_indices() {}

inline audio_bank_processor::
audio_bank_processor::audio_bank_processor(base::block_input_data const& input, audio_bank_state* state) :
_state(state), _data(&audio_bank_data::voice), _relevant_indices_count(), _relevant_indices()
{ update_block_params(input); }

} // namespace inf::base
#endif // INF_PLUGIN_INFERNAL_SYNTH_AUDIO_BANK_PROCESSOR_HPP