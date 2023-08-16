#include <inf.base/shared/support.hpp>
#include <inf.plugin.infernal_synth/shared/support.hpp>
#include <inf.plugin.infernal_synth/cv_bank/processor.hpp>
#include <inf.plugin.infernal_synth/envelope/topology.hpp>
#include <inf.plugin.infernal_synth/envelope/processor.hpp>

#include <cmath>
#include <cassert>

using namespace inf::base;

namespace inf::synth {

// Given control parameter between -1 and 1, returns accumulator 
// A (e.g. 1.0001) and max value M such that A^samples = M.
// Divide by M to get the envelope.
static double
make_slope(double p, std::int32_t samples, double& max)
{
  // Numerical stability issues if slope parameter is outside this range.
  double const slope_min = 0.01;
  double const slope_max = 0.95;
  double slope_limited = slope_min + (slope_max - slope_min) * p;

  // we normalize the linear slope control using pow10 to make it react a bit faster,
  // otherwise a parameter value of 0.5 would still result in a near-linear slope.
  // range goes to infinity as slope parameter goes to 1
  double slope_normalized = 1.0 - std::pow(1.0 - slope_limited, 10.0);
  max = 1.0 / (1.0 - slope_normalized);

  // https://en.wikipedia.org/wiki/Nth_root 
  // -> pow(accumulator, sample_count) = end_value
  // -> accumulator = sample_count-th root of end_value
  // -> accumulator = pow(b, 1 / sample_count * log(b, end_value)) for any base b  
  // works for any slope parameter > 0 (near linear) and < 1 (sharp edge near sample_count) (see limit)
  // and for any for parameter < 0 (near linear) and > -1 (sharp edge near sample 0)
  return std::pow(10.0, 1.0 / samples * std::log10(max));
}

envelope_processor::
envelope_processor(
  base::topology_info const* topology, std::int32_t index,
  float sample_rate, float bpm, base::automation_view const& automation):
part_processor(topology, { part_type::venv, index }, sample_rate),
_bpm(bpm), _released(false), _end_level(0.0), _start_level(0.0), _release_level(0.0), 
_on(), _type(), _mode(), _synced(), _bipolar(), _inverted(), _position(0), 
_hold_samples(0), _delay_samples(0), _sustain(), _decay(), _attack(), _release(), 
_decay1_split(), _decay2_split(), _release1_split(), _release2_split()
{
  automation_view env_automation = automation.rearrange_params({ part_type::venv, index });
  _on = env_automation.block_discrete(envelope_param::on);
  _type = env_automation.block_discrete(envelope_param::type);
  _mode = env_automation.block_discrete(envelope_param::mode);
  _synced = env_automation.block_discrete(envelope_param::synced);
  _bipolar = env_automation.block_discrete(envelope_param::bipolar);
  _inverted = env_automation.block_discrete(envelope_param::invert);

  stage_param_ids decay_ids1 = { envelope_param::decay1_time, envelope_param::decay1_sync, envelope_param::decay1_slope };
  stage_param_ids decay_ids2 = { envelope_param::decay2_time, envelope_param::decay2_sync, envelope_param::decay2_slope };
  stage_param_ids attack_ids1 = { envelope_param::attack1_time, envelope_param::attack1_sync, envelope_param::attack1_slope };
  stage_param_ids attack_ids2 = { envelope_param::attack2_time, envelope_param::attack2_sync, envelope_param::attack2_slope };
  stage_param_ids release_ids1 = { envelope_param::release1_time, envelope_param::release1_sync, envelope_param::release1_slope };
  stage_param_ids release_ids2 = { envelope_param::release2_time, envelope_param::release2_sync, envelope_param::release2_slope };

  _sustain = make_split(env_automation, envelope_param::sustain_level);
  _decay = make_split_stage(env_automation, decay_ids1, envelope_param::decay_split_level, decay_ids2);
  _attack = make_split_stage(env_automation, attack_ids1, envelope_param::attack_split_level, attack_ids2);
  _release = make_split_stage(env_automation, release_ids1, envelope_param::release_split_level, release_ids2);
  _hold_samples = time_or_sync_to_samples(env_automation, envelope_param::hold_time, envelope_param::hold_sync);
  _delay_samples = time_or_sync_to_samples(env_automation, envelope_param::delay_time, envelope_param::delay_sync);

  _decay1_split.level = _sustain.level + _decay.split.level * _sustain.range;
  _decay1_split.range = 1.0 - _decay1_split.level;
  _decay2_split.level = _sustain.level;
  _decay2_split.range = _decay1_split.level - _sustain.level;
  init_release_split(_sustain.level);
}

std::int32_t
envelope_processor::total_dahdr_samples(bool with_release) const
{
  std::int32_t result = 0;
  result += _hold_samples;
  result += _delay_samples;
  result += _decay.stage1.samples;
  result += _decay.stage2.samples;
  result += _attack.stage1.samples;
  result += _attack.stage2.samples;
  if(!with_release) return result;
  result += _release.stage1.samples;
  result += _release.stage2.samples;
  return result;
}

bool
envelope_processor::retrigger(std::int32_t pos, std::int32_t retrig_pos)
{
  if (_mode == envelope_mode::legato || _released || pos != retrig_pos) return false;
  _start_level = _mode == envelope_mode::multi ? _release_level : 0.0;
  _position = 0;
  _end_level = 0.0f;
  _release_level = 0.0f;
  _decay.stage1.value = 1.0;
  _decay.stage2.value = 1.0;
  _attack.stage1.value = 1.0;
  _attack.stage2.value = 1.0;
  _release.stage1.value = 1.0;
  _release.stage2.value = 1.0;
  return true;
}

float
envelope_processor::total_key_samples(float key_time) const
{ 
  float all_samples = static_cast<float>(total_dahdr_samples(true));
  float all_sustain_samples = key_time * sample_rate() + _release.stage1.samples + _release.stage2.samples;
  switch (_type)
  {
  case envelope_type::follow: return all_samples;
  case envelope_type::sustain: return all_sustain_samples;
  case envelope_type::release: return std::min(all_sustain_samples, all_samples);
  default: assert(false); return 0.0f;
  }
}

void
envelope_processor::init_release_split(double release_level)
{
  _release1_split.level = _release.split.level * release_level;
  _release1_split.range = release_level - _release1_split.level;
  _release2_split.level = 0.0;
  _release2_split.range = _release1_split.level;
}

envelope_stage
envelope_processor::make_stage(automation_view const& automation, stage_param_ids ids) const
{
  double slope_max;
  envelope_stage result = {};
  result.value = 1.0;
  result.samples = time_or_sync_to_samples(automation, ids.time, ids.sync);
  if (result.samples == 0) return result;
  float slope = automation.block_real_transform(ids.slope);
  result.accumulator = make_slope(slope, result.samples, slope_max);
  result.inv_slope_range = 1.0 / (slope_max - 1.0);
  return result;
}

envelope_split
envelope_processor::make_split(automation_view const& automation, std::int32_t split_param) const
{
  envelope_split result;
  result.level = automation.block_real_transform(split_param);
  result.range = 1.0 - result.level;
  return result;
}

std::int32_t
envelope_processor::time_or_sync_to_samples(automation_view const& automation, std::int32_t time, std::int32_t sync) const
{
  if (_synced == 0)
    return static_cast<std::int32_t>(automation.block_real_transform(time) * sample_rate());
  float timesig = envelope_timesig_values[automation.block_discrete(sync)];
  return static_cast<std::int32_t>(timesig_to_samples(sample_rate(), _bpm, timesig));
}

envelope_split_stage
envelope_processor::make_split_stage(automation_view const& automation, stage_param_ids ids1, std::int32_t split, stage_param_ids ids2) const
{
  envelope_split_stage result = {};
  result.split = make_split(automation, split);
  result.stage1 = make_stage(automation, ids1);
  result.stage2 = make_stage(automation, ids2);
  return result;
}

envelope_output
envelope_processor::process(envelope_input const& input, cv_buffer& buffer)
{
  std::int32_t hold_end;
  std::int32_t delay_end;
  std::int32_t decay1_end;
  std::int32_t decay2_end;
  std::int32_t attack1_end;
  std::int32_t attack2_end;
  std::int32_t release1_end;
  std::int32_t release2_end;

  double env_val;
  float* out = buffer.values;
  envelope_output result = { false, 0 };

  // If released this buffer, 0 <= release_sample < sample_count.
  assert(0 <= input.release_sample && input.release_sample <= input.block->sample_count);

  // Always run envelope 0, it's hardwired to the voice amp section.
  if (id().index > 0 && _on == 0)
  {
    buffer.flags = cv_flags();
    std::memset(out, 0, input.block->sample_count * sizeof(float));
    return result;
  }

  std::int32_t s = input.offset;
  buffer.flags = { _bipolar != 0, _inverted != 0 };
  std::int32_t release_sample = input.release_sample;
  std::int64_t start_time = performance_counter();

  // Follow ignores release, just follow the envelope.
  if(_type == envelope_type::follow)
    release_sample = input.block->sample_count;

  // Delay.
  delay_end = _delay_samples;
  for(; _position < delay_end && s < release_sample; _position++, s++)
  {
    if(retrigger(s, input.retrig_pos)) goto retriggered;
    out[s] = static_cast<float>(_end_level = _release_level = _start_level);
  }

  // Attack 1.
  attack1_end = delay_end + _attack.stage1.samples;
  for(; _position < attack1_end && s < release_sample; _position++, s++)
  {
    if (retrigger(s, input.retrig_pos)) goto retriggered;
    env_val = _attack.split.level * sanity_unipolar(_attack.stage1.next());
    out[s] = static_cast<float>(_end_level = _release_level = _start_level + (1.0f - _start_level) * env_val);
  }

  // Attack 2.
  attack2_end = attack1_end + _attack.stage2.samples;
  for (; _position < attack2_end && s < release_sample; _position++, s++)
  {
    if (retrigger(s, input.retrig_pos)) goto retriggered;
    env_val = _attack.split.level + _attack.split.range * sanity_unipolar(_attack.stage2.next());
    out[s] = static_cast<float>(_end_level = _release_level = _start_level + (1.0f - _start_level) * env_val);
  }

  // Hold.
  hold_end = attack2_end + _hold_samples;
  for (; _position < hold_end && s < release_sample; _position++, s++)
  {
    if (retrigger(s, input.retrig_pos)) goto retriggered;
    out[s] = static_cast<float>(_end_level = _release_level = 1.0);
  }

  // Decay 1.
  decay1_end = hold_end + _decay.stage1.samples;
  for (; _position < decay1_end && s < release_sample; _position++, s++)
  {
    if (retrigger(s, input.retrig_pos)) goto retriggered;
    env_val = sanity_unipolar(_decay1_split.level + _decay1_split.range * (1.0 - _decay.stage1.next()));
    out[s] = static_cast<float>(_end_level = _release_level = env_val);
  }

  // Decay 2.
  decay2_end = decay1_end + _decay.stage2.samples;
  for (; _position < decay2_end && s < release_sample; _position++, s++)
  {
    if (retrigger(s, input.retrig_pos)) goto retriggered;
    env_val = sanity_unipolar(_decay2_split.level + _decay2_split.range * (1.0 - _decay.stage2.next()));
    out[s] = static_cast<float>(_end_level = _release_level = env_val);
  }

  // Sustain.
  if (_type == envelope_type::sustain && !_released)
    for(; s < release_sample; s++)
    {
      if (retrigger(s, input.retrig_pos)) goto retriggered;
      out[s] = static_cast<float>(_end_level = _release_level = _sustain.level);
    }

  // Starting release section,
  // Except follow which just unconditionally follows the envelope stages.
  if (release_sample < input.block->sample_count && s == release_sample && !_released)
  {
    _released = true;
    _position = decay2_end;
    init_release_split(_release_level);
  }

  // Release 1.
  release1_end = decay2_end + _release.stage1.samples;
  for (; _position < release1_end && s < input.block->sample_count; _position++, s++)
  {
    if (retrigger(s, input.retrig_pos)) goto retriggered;
    env_val = sanity_unipolar(_release1_split.level + _release1_split.range * (1.0 - _release.stage1.next()));
    out[s] = static_cast<float>(_end_level = env_val);
  }
   
  // Release 2.
  release2_end = release1_end + _release.stage2.samples;
  for (; _position < release2_end && s < input.block->sample_count; _position++, s++)
  {
    if (retrigger(s, input.retrig_pos)) goto retriggered;
    env_val = sanity_unipolar(_release2_split.level + _release2_split.range * (1.0 - _release.stage2.next()));
    out[s] = static_cast<float>(_end_level = env_val);
  }

  // End.
  if (_position == release2_end)
  {
    result.ended = true;
    std::fill(out + s, out + input.block->sample_count, static_cast<float>(_end_level));
  } 

  // Regular return.
  unipolar_transform(buffer, input.block->sample_count);
  result.own_time = performance_counter() - start_time;
  return result;

  // Retriggered: state has been reset to zero, need to fill the rest of the buffer with the newly started envelope now.
retriggered:
  envelope_input retrig_input;
  retrig_input.retrig_pos = -1;
  retrig_input.block = input.block;
  retrig_input.offset = input.retrig_pos;
  retrig_input.release_sample = release_sample;
  result = process(retrig_input, buffer);
  unipolar_transform(buffer, input.block->sample_count);
  result.own_time = performance_counter() - start_time;
  return result;
}

} // namespace inf::synth