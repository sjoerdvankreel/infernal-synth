#ifndef INF_SYNTH_ENVELOPE_PROCESSOR_HPP
#define INF_SYNTH_ENVELOPE_PROCESSOR_HPP

#include <inf.base/plugin/state.hpp>
#include <inf.synth/envelope/state.hpp>
#include <inf.synth/shared/part_processor.hpp>

#include <cstdint>

namespace inf::synth {

// Per-voice envelope generator.
class envelope_processor:
public part_processor
{  
  float _bpm;
  bool _released;
  double _end_level;
  double _start_level;
  double _release_level;

  std::int32_t _on;
  std::int32_t _type;
  std::int32_t _mode;
  std::int32_t _synced;
  std::int32_t _bipolar;
  std::int32_t _inverted;

  std::int32_t _position;
  std::int32_t _hold_samples;
  std::int32_t _delay_samples;

  envelope_split _sustain;
  envelope_split_stage _decay;
  envelope_split_stage _attack;
  envelope_split_stage _release;

  envelope_split _decay1_split;
  envelope_split _decay2_split;
  envelope_split _release1_split;
  envelope_split _release2_split;

public:
  envelope_processor() = default;
  envelope_processor(
    base::topology_info const* topology, std::int32_t index, 
    float sample_rate, float bpm, base::automation_view const& automation);

public:
  float total_key_samples(float key_time) const;
  std::int32_t total_dahdr_samples(bool with_release) const;
  envelope_output process(envelope_input const& input, struct cv_buffer& buffer);

private:  
  void init_release_split(double release_level);
  bool retrigger(std::int32_t pos, std::int32_t retrig_pos);
  envelope_split make_split(base::automation_view const& automation, std::int32_t split) const;
  envelope_stage make_stage(base::automation_view const& automation, stage_param_ids param_ids) const;
  std::int32_t time_or_sync_to_samples(base::automation_view const& automation, std::int32_t time, std::int32_t sync) const;
  envelope_split_stage make_split_stage(base::automation_view const& automation, stage_param_ids ids1, std::int32_t split, stage_param_ids ids2) const;
};

} // namespace inf::synth
#endif // INF_SYNTH_ENVELOPE_PROCESSOR_HPP