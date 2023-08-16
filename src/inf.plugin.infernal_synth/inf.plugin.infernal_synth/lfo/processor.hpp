#ifndef INF_SYNTH_LFO_PROCESSOR_HPP
#define INF_SYNTH_LFO_PROCESSOR_HPP

#include <inf.synth/lfo/state.hpp>
#include <inf.synth/shared/part_processor.hpp>
#include <inf.synth/shared/basic_lp_filter.hpp>

namespace inf::synth {

struct scratch_space;

// Both voice and global.
class lfo_processor:
public part_processor
{
  bool _ended;
  bool _started;
  std::int32_t _end_index;

  std::int32_t _on;
  std::int32_t _type;
  std::int32_t _single;
  std::int32_t _synced;
  std::int32_t _bipolar;
  std::int32_t _inverted;

  float _phase;
  float _max_freq;
  float _filter_amt;
  float _end_sample;
  float _synced_frequency;

  lfo_free_state _free;
  lfo_basic_state _basic;
  lfo_random_state _random;
  basic_lp_filter _filter;

  void update_block_basic(base::automation_view const& automation);
  void update_block_random(base::automation_view const& automation);
  void update_block_free(base::automation_view const& automation);
  void update_block_params(base::automation_view const& automation, float bpm);

  void init_random(base::automation_view const& automation);
  void process_basic(float* rate, float* out, std::int32_t sample_count);
  void process_filter(float* out, std::int32_t sample_count, scratch_space& scratch);

  template <class processor_type>
  void process(float* rate, float* out, std::int32_t sample_count, processor_type processor);

public:
  // Global and voice.
  std::int64_t process_any(base::block_input_data const& input, cv_buffer& buffer, scratch_space& scratch); 
  std::int64_t process_global(base::block_input_data const& input, cv_buffer& buffer, scratch_space& scratch);

  lfo_processor() = default;
  lfo_processor(base::topology_info const* topology, base::part_id id, float sample_rate, float bpm, base::automation_view const& automation);
};

} // namespace inf::synth
#endif // INF_SYNTH_LFO_PROCESSOR_HPP