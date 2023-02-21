#include <inf.synth/lfo/topology.hpp>
#include <inf.synth/lfo/processor.hpp>
#include <inf.synth/lfo/processor_basic.hpp>
#include <inf.synth/lfo/processor_custom.hpp>
#include <inf.synth/lfo/processor_random.hpp>
#include <inf.synth/shared/scratch_space.hpp>

#include <cmath>
#include <cassert>

using namespace inf::base;

namespace inf::synth {

struct scratch_space;

// Voice and global.
lfo_processor::
lfo_processor(topology_info const* topology, part_id id, float sample_rate, float bpm, automation_view const& automation) :
part_processor(topology, id, sample_rate), _ended(false), _started(false), _end_index(0), 
_on(), _type(), _single(), _synced(), _bipolar(), _inverted(), _phase(0.0f), _max_freq(), 
_filter_amt(), _end_sample(), _synced_frequency(), _basic(), _random(), _custom(), _filter()
{ 
  automation_view lfo_automation = automation.rearrange_params(id);
  _type = lfo_automation.block_discrete(lfo_param::type);
  if (_type == lfo_type::random) init_random(lfo_automation);
  update_block_params(lfo_automation, bpm);
  if(_type == lfo_type::random) _random.reset();
}

void
lfo_processor::init_random(automation_view const& automation)
{
  _random.seed_x = automation.block_discrete(lfo_param::rand_seedx);
  _random.seed_y = automation.block_discrete(lfo_param::rand_seedy);
  _random.state_x = std::numeric_limits<uint32_t>::max() / _random.seed_x;
  _random.state_y = std::numeric_limits<uint32_t>::max() / _random.seed_y;
}

void
lfo_processor::update_block_params(automation_view const& automation, float bpm)
{
  std::int32_t tempo = automation.block_discrete(lfo_param::tempo);
  _on = automation.block_discrete(lfo_param::on);
  _type = automation.block_discrete(lfo_param::type);
  _synced = automation.block_discrete(lfo_param::synced);
  _single = automation.block_discrete(lfo_param::single);
  _bipolar = automation.block_discrete(lfo_param::bipolar);
  _inverted = automation.block_discrete(lfo_param::invert);
  _filter_amt = automation.block_real_transform(lfo_param::filter);
  _synced_frequency = timesig_to_frequency(bpm, lfo_timesig_values[tempo]);

  switch (_type)
  {
  case lfo_type::basic: update_block_basic(automation); break;
  case lfo_type::random: update_block_random(automation); break;
  case lfo_type::custom: update_block_custom(automation); break;
  default: assert(false); break;
  }
}

void 
lfo_processor::update_block_basic(automation_view const& automation)
{
  _basic.type = automation.block_discrete(lfo_param::basic_type);
  _basic.pw = automation.block_real_transform(lfo_param::basic_pw);
  _basic.offset = automation.block_real_transform(lfo_param::basic_offset);
}

void 
lfo_processor::update_block_random(automation_view const& automation)
{
  float step_total = 0.0f;
  float const distrib_min = 0.1f;
  float const distrib_max = 0.9f;

  _random.type = automation.block_discrete(lfo_param::rand_type);
  _random.steps = automation.block_discrete(lfo_param::rand_steps);
  _random.seed_x = automation.block_discrete(lfo_param::rand_seedx);
  _random.seed_y = automation.block_discrete(lfo_param::rand_seedy);
  _random.rand_steps = automation.block_discrete(lfo_param::rand_rand_steps);
  _random.amt = automation.block_real_transform(lfo_param::rand_amt);
  if (_random.rand_steps == 0)
  {
    for (std::int32_t i = 0; i < _random.steps; i++)
      _random.distribution[i] = 1.0f / static_cast<float>(_random.steps);
    return;
  }
  for (std::int32_t i = 0; i < _random.steps; i++)
    _random.distribution[i] = distrib_min + (distrib_max - distrib_min) * fast_rand_next(_random.state_x);
  for (std::int32_t i = 0; i < _random.steps; i++)
    step_total += _random.distribution[i];
  for (std::int32_t i = 0; i < _random.steps; i++)
    _random.distribution[i] /= step_total;
  for (std::int32_t i = 0; i < _random.steps; i++)
    assert(0.0f <= _random.distribution[i] && _random.distribution[i] <= 1.0f);
}

void 
lfo_processor::update_block_custom(automation_view const& automation)
{
  float total;
  float hold1, hold2;
  float fall1, fall2;
  float rise1, rise2;
  float delay1, delay2;

  hold1 = automation.block_real_transform(lfo_param::custom_hold1);
  hold2 = automation.block_real_transform(lfo_param::custom_hold2);
  fall1 = automation.block_real_transform(lfo_param::custom_fall1);
  fall2 = automation.block_real_transform(lfo_param::custom_fall2);
  rise1 = automation.block_real_transform(lfo_param::custom_rise1);
  rise2 = automation.block_real_transform(lfo_param::custom_rise2);
  delay1 = automation.block_real_transform(lfo_param::custom_delay1);
  delay2 = automation.block_real_transform(lfo_param::custom_delay2);
  total = hold1 + hold2 + fall1 + fall2 + rise1 + rise2 + delay1 + delay2;
  _custom.rise1.factor = rise1 / total;
  _custom.rise2.factor = rise2 / total;
  _custom.fall1.factor = fall1 / total;
  _custom.fall2.factor = fall2 / total;
  _custom.hold1_factor = hold1 / total;
  _custom.hold2_factor = hold2 / total;
  _custom.delay1_factor = delay1 / total;
  _custom.delay2_factor = delay2 / total;
  _custom.fall1.slope = automation.block_discrete(lfo_param::custom_fall1_slope);
  _custom.fall2.slope = automation.block_discrete(lfo_param::custom_fall2_slope);
  _custom.rise1.slope = automation.block_discrete(lfo_param::custom_rise1_slope);
  _custom.rise2.slope = automation.block_discrete(lfo_param::custom_rise2_slope);
  _custom.enabled = total >= 0.01;
}

// processor_type, please be inlined
template <class processor_type> void
lfo_processor::process(float* rate, float* out, std::int32_t sample_count, processor_type processor)
{
  // Fill buffer untill we hit end of one-shot.
  // Keep track of highest frequency for filter.
  _max_freq = 0.0f;
  _end_index = sample_count;
  for (std::int32_t s = 0; s < sample_count; s++)
  {
    _max_freq = std::max(_max_freq, rate[s]);
    out[s] = sanity_unipolar(processor(rate[s], _phase));
    _phase += rate[s] / sample_rate();
    if (_phase >= 1.0f && _single != 0) { _end_index = s; break; }
    _phase -= std::floor(_phase);
  }
}

void
lfo_processor::process_filter(float* out, std::int32_t sample_count, scratch_space& scratch)
{
  // Prevent filter startup from zero.
  if (!_started)
  {
    _started = true;
    _filter.in1 = out[0];
    _filter.out1 = out[0];
  }

  // Smooth output using filter param.
  // 0 = no filtering, > 0 = cutoff at 1hz + (1 - filter) * 2 * lfo frequency.
  // https://www.musicdsp.org/en/latest/Filters/117-one-pole-one-zero-lp-hp.html
  if (_filter_amt < 1.0f)
  {
    float cutoff_hz = 1.0f + _filter_amt * _max_freq * 2.0f;
    _filter.init(sample_rate(), cutoff_hz);
    float* unfiltered_scratch = scratch.storage_f32.buffer(0);
    std::copy(out, out + sample_count, unfiltered_scratch);
    for (std::int32_t s = 0; s < sample_count; s++)
      out[s] = sanity_unipolar(_filter.next(unfiltered_scratch[s]));
  }
}

void 
lfo_processor::process_basic(float* rate, float* out, std::int32_t sample_count)
{
  switch (_basic.type)
  {
  case lfo_basic_type::saw: process(rate, out, sample_count, lfo_saw_processor({ _basic.offset })); break;
  case lfo_basic_type::sine: process(rate, out, sample_count, lfo_sine_processor({ _basic.offset })); break;
  case lfo_basic_type::triangle: process(rate, out, sample_count, lfo_triangle_processor({ _basic.offset })); break;
  case lfo_basic_type::pulse: process(rate, out, sample_count, lfo_pulse_processor({ _basic.offset, _basic.pw })); break;
  default: assert(false); break;
  }
}

std::int64_t
lfo_processor::process_global(block_input_data const& input, cv_buffer& buffer, scratch_space& scratch)
{
  // Project reset, be sure to start over from scratch.
  if (input.stream_position == 0)
  {
    _random.reset();
    _phase = 0.0f;
  }

  // Global level lfo updates block params once per block.
  automation_view automation(input.automation.rearrange_params(id()));
  update_block_params(automation, input.bpm);
  return process_any(input, buffer, scratch);
}

// Global and voice.
std::int64_t 
lfo_processor::process_any(block_input_data const& input, cv_buffer& buffer, scratch_space& scratch)
{
  buffer.flags = { _bipolar != 0, _inverted != 0 };

  if (!_on)
  { 
    std::memset(buffer.values, 0, input.sample_count * sizeof(float));
    return 0;
  }    

  // Shortcut if we're done already.
  _ended &= _single != 0;
  if (_ended)
  {     
    std::fill(buffer.values, buffer.values + input.sample_count, _end_sample);
    return 0;
  }
   
  std::int64_t start_time = performance_counter();

  // Use single rate buffer even if fixed for block length.
  automation_view automation(input.automation.rearrange_params(id()));
  float* rate = scratch.storage_f32.buffer(0);
  if(_synced == 0)
    automation.continuous_real_transform(lfo_param::rate, rate, input.sample_count);
  else
    std::fill(rate, rate + input.sample_count, _synced_frequency);
    
  // Run generator.
  switch (_type)
  {
  case lfo_type::basic: process_basic(rate, buffer.values, input.sample_count); break;
  case lfo_type::random: process(rate, buffer.values, input.sample_count, lfo_random_processor({ sample_rate(), &_random})); break;
  case lfo_type::custom: process(rate, buffer.values, input.sample_count, lfo_custom_processor({ sample_rate(), &_custom })); break;
  default: assert(false); break;
  }

  // Fill remainder with last sample.
  float* out = buffer.values; 
  if (_end_index != input.sample_count)
  {
    _ended = true;
    _end_sample = out[_end_index];
    std::fill(out + _end_index, out + input.sample_count, _end_sample);
  }

  // Filter and transform.
  process_filter(out, input.sample_count, scratch);  
  unipolar_transform(buffer, input.sample_count);
  return performance_counter() - start_time;
}

} // namespace inf::synth