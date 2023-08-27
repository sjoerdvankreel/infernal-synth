#ifndef INF_PLUGIN_INFERNAL_SYNTH_DSP_CV_BANK_PROCESSOR_HPP
#define INF_PLUGIN_INFERNAL_SYNTH_DSP_CV_BANK_PROCESSOR_HPP

#include <inf.plugin.infernal_synth/cv_bank/state.hpp>
#include <inf.plugin.infernal_synth/cv_bank/topology.hpp>

namespace inf::plugin::infernal_synth {

// Combined CV routing, either voice or global.
// Handles both modulation and transformation from [0, 1] to dsp range (e.g. 20-20000 for filter freq).
// All modulation is done in [0, 1], final result is clipped.
class cv_bank_processor
{
  cv_bank_state* _state;
  cv_bank_data const* _data;
  base::topology_info const* _topology;

public:
  cv_bank_processor() = default;
  cv_bank_processor(base::topology_info const* topology, cv_bank_state* state); // Global
  cv_bank_processor(base::topology_info const* topology, cv_bank_state* state, 
    cv_hold_sample const* gcv_uni_hold_, cv_hold_sample const* gcv_bi_hold_, 
    cv_hold_sample const* glfo_hold_, float velo, 
    std::int32_t midi, base::block_input_data const& input); // Voice

  // Once for voice, per-block for global.
  void update_block_params(base::block_input_data const& input);
  void apply_voice_state(
    cv_hold_sample const* gcv_uni_hold, cv_hold_sample const* gcv_bi_hold, 
    cv_hold_sample const* glfo_hold, float velo, std::int32_t midi, std::int32_t sample_count);

  // Mixdown specified targets (e.g. "continuous filter 1 params").
  std::int64_t modulate(cv_bank_input const& input, float const* const*& result);

private:
  void apply_modulation(
    cv_bank_input const& input, base::automation_view const& bank_automation, 
    cv_route_indices const& indices, std::int32_t mapped_target);

  bool input_bipolar(std::int32_t input, std::int32_t index) const;
  bool input_bipolar_voice(std::int32_t input, std::int32_t index) const;
  bool input_bipolar_global(std::int32_t input, std::int32_t index) const;
  
  float const* input_buffer(std::int32_t input, std::int32_t index) const;
  float const* input_buffer_voice(std::int32_t input, std::int32_t index) const;
  float const* input_buffer_global(std::int32_t input, std::int32_t index) const;
  
  // Precomputing all relevant routes for each target output.
  static inline std::int32_t constexpr max_total_route_output_count = std::max(vcv_route_output_total_count, gcv_route_output_total_count);

  std::array<std::int32_t, max_total_route_output_count> _relevant_indices_count;
  std::array<std::array<cv_route_indices, cv_bank_route_count>, max_total_route_output_count> _relevant_indices;
};

} // namespace inf::plugin::infernal_synth
#endif // INF_PLUGIN_INFERNAL_SYNTH_DSP_CV_BANK_PROCESSOR_HPP