#include <inf.base/shared/support.hpp>
#include <inf.synth/cv_bank/topology.hpp>
#include <inf.synth/audio_bank/state.hpp>
#include <inf.synth/oscillator/config.hpp>
#include <inf.synth/audio_bank/topology.hpp>

using namespace inf::base;

namespace inf::synth {

audio_bank_data const audio_bank_data::voice =
{
  part_type::vaudio_bank, vaudio_bank_count, vcv_route_output::vaudio_bank, vaudio_route_output_total_count, 0,
  multi_list_table_init_in(vaudio_route_input_counts, vaudio_route_input::count),
  multi_list_table_init_in(vaudio_route_output_counts, vaudio_route_output::count),
  multi_list_table_init_out(vaudio_route_input_counts, vaudio_route_input::count)
};

audio_bank_data const audio_bank_data::global =
{
  part_type::gaudio_bank, gaudio_bank_count, gcv_route_output::gaudio_bank, gaudio_route_output_total_count, 1,
  multi_list_table_init_in(gaudio_route_input_counts, gaudio_route_input::count),
  multi_list_table_init_in(gaudio_route_output_counts, gaudio_route_output::count),
  multi_list_table_init_out(gaudio_route_input_counts, gaudio_route_input::count)
};

audio_bank_state::audio_bank_state(std::int32_t max_sample_count):
out(stereo_channels, max_sample_count), voice(stereo_channels, max_sample_count),
voices(stereo_channels, max_sample_count), external(stereo_channels, max_sample_count),
vosc(), veffect(), geffect(), vosc_reset_pos(), vosc_reset()
{
  for (std::int32_t i = 0; i < veffect_count; i++)
    veffect[i] = cont_storage<float>(stereo_channels, max_sample_count);
  for (std::int32_t i = 0; i < geffect_count; i++)
    geffect[i] = cont_storage<float>(stereo_channels, max_sample_count);
  for (std::int32_t i = 0; i < vosc_count; i++)
  {
    vosc[i] = cont_storage<float>(stereo_channels, max_sample_count);
    vosc_reset_pos[i] = cont_storage<float>(osc_max_voices, max_sample_count);
    vosc_reset[i] = cont_storage<std::int32_t>(osc_max_voices, max_sample_count);
  }
}

} // namespace inf::synth 