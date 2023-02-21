#ifndef INF_SYNTH_VOICE_MASTER_PROCESSOR_HPP
#define INF_SYNTH_VOICE_MASTER_PROCESSOR_HPP

#include <inf.synth/voice_master/state.hpp>
#include <inf.synth/shared/audio_part_processor.hpp>
#include <cstdint>

namespace inf::synth {

class cv_bank_processor;

// Voice and master level and panning.
class amp_bal_processor:
public audio_part_processor
{
  static std::int32_t modulation_type(std::int32_t part_type);
public:
  amp_bal_processor() = default;
  amp_bal_processor(base::topology_info const* topology, std::int32_t part_type, float sample_rate);
  audio_part_output process(amp_bal_input const& input, float* const* audio_out, cv_bank_processor& cv);
};

inline std::int32_t 
amp_bal_processor::modulation_type(std::int32_t part_type)
{
  if(part_type == part_type::voice) 
    return vcv_route_output::voice;
  return gcv_route_output::master;
}

inline amp_bal_processor::
amp_bal_processor::amp_bal_processor(base::topology_info const* topology, std::int32_t part_type, float sample_rate):
audio_part_processor(topology, { part_type, 0 }, sample_rate, modulation_type(part_type)) {}

} // namespace inf::synth
#endif // INF_SYNTH_VOICE_MASTER_PROCESSOR_HPP