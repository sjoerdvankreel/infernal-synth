#ifndef SVN_SYNTH_VOICE_MASTER_STATE_HPP
#define SVN_SYNTH_VOICE_MASTER_STATE_HPP

#include <svn.base/processor/state.hpp>
#include <cstdint>

namespace svn::synth {

struct amp_bal_input
{
  float const* amp_env;
  float const* const* audio_in;
  base::block_input_data const* block;
};

} // namespace svn::synth
#endif // SVN_SYNTH_VOICE_MASTER_STATE_HPP