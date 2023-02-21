#include <svn.base/shared/support.hpp>
#include <svn.synth/shared/support.hpp>
#include <svn.synth/cv_bank/processor.hpp>
#include <svn.synth/voice_master/processor.hpp>

#include <cassert>

using namespace svn::base;

namespace svn::synth {

audio_part_output 
amp_bal_processor::process(amp_bal_input const& input, float* const* audio_out, cv_bank_processor& cv)
{  
  float const* const* params;
  audio_part_output result = { };

  // Modulate.
  cv_bank_input cv_input;
  automation_view automation(input.block->automation.rearrange_params(id()));
  cv_input.block = input.block;
  cv_input.automated = &automation;
  cv_input.modulation_id = modulation_id();
  result.cv_time = cv.modulate(cv_input, params);

  float const* env = input.amp_env;
  float const* const* in = input.audio_in;
  float const* bal = params[vgamp_param::bal];
  float const* gain = params[vgamp_param::gain];

  // Apply gain and balance.
  std::int64_t start_time = performance_counter();
  for(std::int32_t c = 0; c < stereo_channels; c++)
    for (std::int32_t s = 0; s < input.block->sample_count; s++)
      audio_out[c][s] = gain[s] * env[s] * in[c][s] * stereo_balance(c, bal[s]);
  result.own_time = performance_counter() - start_time;
  return result;
}

} // namespace svn::synth