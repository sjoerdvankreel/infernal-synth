#ifndef INF_PLUGIN_INFERNAL_SYNTH_AMP_STATE_HPP
#define INF_PLUGIN_INFERNAL_SYNTH_AMP_STATE_HPP

#include <inf.base/plugin/state.hpp>
#include <cstdint>

namespace inf::synth {

struct amp_bal_input
{
  float const* amp_env;
  float const* const* audio_in;
  base::block_input_data const* block;
};

} // namespace inf::synth
#endif // INF_PLUGIN_INFERNAL_SYNTH_AMP_STATE_HPP