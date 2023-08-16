#include <inf.plugin.infernal_synth/oscillator/state.hpp>

namespace inf::plugin::infernal_synth {

oscillator_state::
oscillator_state(float sample_rate):
phases(), unsynced_phases(), sync_unprocessed_samples(),
kps_initialized(false), max_kps_length(0), kps_positions(), kps_lines(),
noise_prev_draw(), noise_prev_draw_phase(), noise_rand_state_x(), 
noise_rand_state_y(), noise_color_value(), noise_color_hold()
{
  float const kps_min_freq = 20.0f;
  max_kps_length = static_cast<std::int32_t>(std::ceil(sample_rate / kps_min_freq));
  for(std::int32_t i = 0; i < osc_max_voices; i++)
    kps_lines[i] = std::vector<float>(max_kps_length);
}

} // namespace inf::plugin::infernal_synth