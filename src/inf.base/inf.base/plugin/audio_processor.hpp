#ifndef INF_BASE_PLUGIN_AUDIO_PROCESSOR_HPP
#define INF_BASE_PLUGIN_AUDIO_PROCESSOR_HPP

#include <inf.base/plugin/state.hpp>
#include <inf.base/shared/cont_storage.hpp>
#include <inf.base/topology/topology_info.hpp>

#include <vector>
#include <cstdint>

namespace inf::base {

class audio_processor
{
private:
  block_input _input;
  block_output _output;
  cont_storage<float> _silence;
  std::vector<note_event> _notes;

  float const _sample_rate;
  // Of size parameter count (so unused indices at continuous parameters).
  // Block params are block, voice, fixed and output.
  param_value* const _state;
  // Changed parameters 0/1 for this block, be it discrete, continuous, voice, block, whatever.
  // Can't be bool, because it's passed down from std::vector.
  std::int32_t* const _changed;
  std::int32_t _prev_block_size = 0;
  std::int64_t _prev_start_perf_count = 0;

  topology_info const* const _topology;
  // Of size parameter count (so nullptr at block parameter indices).
  std::vector<float*> _continuous_automation;
  // Of size continuous parameter count * max sample count, so does NOT align with total parameter count.
  std::vector<float> _continuous_automation_buffer;

private:
  void automation_check();
  void audio_check(float const* const* audio);

protected:
  virtual void process(
    block_input const& input, block_output& output) = 0;

public:
  float sample_rate() const { return _sample_rate; }
  topology_info const* topology() const { return _topology; }

  virtual ~audio_processor() = default;
  audio_processor(topology_info const* topology, float sample_rate, 
    std::int32_t max_sample_count, param_value* state, std::int32_t* changed);

  // Caller writes either discrete or normalized (0..1) values to automation.
  block_input& prepare_block(std::int32_t sample_count);
  // Process block call overwrites real valued automation with actual range.
  block_output const& process(
    float const* const* input, float* const* output, bool hard_reset,
    std::int64_t prev_end_perf_count, std::int64_t new_start_perf_count);
};

} // namespace inf::base
#endif // INF_BASE_PLUGIN_AUDIO_PROCESSOR_HPP