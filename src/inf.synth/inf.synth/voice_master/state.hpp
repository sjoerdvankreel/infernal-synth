#ifndef INF_SYNTH_VOICE_MASTER_STATE_HPP
#define INF_SYNTH_VOICE_MASTER_STATE_HPP

#include <inf.base/processor/state.hpp>
#include <cstdint>

namespace inf::synth {

struct amp_bal_input
{
  float const* amp_env;
  float const* const* audio_in;
  base::block_input_data const* block;
};

} // namespace inf::synth
#endif // INF_SYNTH_VOICE_MASTER_STATE_HPP