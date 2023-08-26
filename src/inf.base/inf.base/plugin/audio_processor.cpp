#include <inf.base/shared/state.hpp>
#include <inf.base/shared/support.hpp>
#include <inf.base/plugin/audio_processor.hpp>
#include <cassert>

namespace inf::base {

audio_processor::
audio_processor(
topology_info const* topology, float sample_rate,
std::int32_t max_sample_count, param_value* state, std::int32_t* changed):
_input(), _output(), _silence(stereo_channels, max_sample_count),
_sample_rate(sample_rate), _state(state), _changed(changed), _topology(topology),
_continuous_automation(topology->params.size()),
_continuous_automation_buffer(static_cast<std::size_t>(topology->continuous_param_count * max_sample_count))
{
  assert(state != nullptr);
  assert(sample_rate > 0.0f);
  assert(topology != nullptr);
  assert(max_sample_count > 0);

  _input.note_input_event_count = 0;
  _input.automation_input_event_count = 0;
  _input.block_automation_raw = _state;
  _input.changed_automation_raw = _changed;
  _output.block_automation_raw = _state;

  std::int32_t continuous = 0;
  for (std::int32_t p = 0; p < topology->input_param_count; p++)
    if(_topology->params[p].descriptor->data.is_continuous())
      _continuous_automation[p] = _continuous_automation_buffer.data() + continuous++ * max_sample_count;
  _input.continuous_automation_raw = _continuous_automation.data();
}

void 
audio_processor::audio_check(float const* const* audio)
{
  for (std::int32_t c = 0; c < _input.channel_count; c++)
    for (std::int32_t s = 0; s < _input.data.sample_count; s++)
      sanity(audio[c][s]);
}

void
audio_processor::automation_check()
{
  // Renoise comes with values slightly out of bounds, maybe because of interpolation?
  // Cannot clamp() because that is a major performance hit.
  float const epsilon = 1.0E-5f;
  (void) epsilon; // Debug assert.
  for (std::int32_t p = 0; p < _topology->input_param_count; p++)
  {
    auto const& descriptor = *_topology->params[p].descriptor;
    if(descriptor.data.is_continuous())
      for (std::int32_t s = 0; s < _input.data.sample_count; s++)
        assert(-epsilon <= _input.continuous_automation_raw[p][s] &&
          _input.continuous_automation_raw[p][s] <= 1.0f + epsilon);
    else if(descriptor.data.type == param_type::real)
      assert(-epsilon <= _input.block_automation_raw[p].real &&
        _input.block_automation_raw[p].real <= 1.0f + epsilon);
    else
      assert(descriptor.data.discrete.min <= _input.block_automation_raw[p].discrete 
        && _input.block_automation_raw[p].discrete <= descriptor.data.discrete.max);
  }
}

block_input&
audio_processor::prepare_block(std::int32_t sample_count)
{
  _input.note_input_event_count = 0;
  _input.automation_input_event_count = 0;
  for (std::int32_t p = _topology->input_param_count; p < _topology->params.size(); p++)
    _output.block_automation_raw[p] = _topology->params[p].descriptor->data.default_value();
  for (std::int32_t p = 0; p < _topology->input_param_count; p++)
    if (_topology->params[p].descriptor->data.kind == param_kind::continuous)
      std::fill(_continuous_automation[p], _continuous_automation[p] + sample_count, _state[p].real);
    else if (_topology->params[p].descriptor->data.kind == param_kind::fixed)
      std::fill(_continuous_automation[p], _continuous_automation[p] + sample_count,
        _topology->params[p].descriptor->data.real.default_);
  return _input;
}

block_output const&
audio_processor::process(
  float const* const* input, float* const* output, bool hard_reset,
  std::int64_t prev_end_perf_count, std::int64_t new_start_perf_count)
{
  _topology->state_check(_state);
  _output.audio = output;
  _input.data.audio = _silence.buffers();
  if(!_topology->is_instrument()) _input.data.audio = input;
  _input.data.automation = automation_view(
    topology(), _input.block_automation_raw, _input.continuous_automation_raw, nullptr,
    topology()->input_param_count, topology()->input_param_count, 0, 
    _input.data.sample_count, 0, _input.data.sample_count);
  automation_check();
  audio_check(_input.data.audio);
  std::uint64_t state = disable_denormals();
  _input.hard_reset = hard_reset;
  _input.prev_perf_count = prev_end_perf_count - _prev_start_perf_count;
  _input.prev_sample_count = _prev_block_size == -1? _input.data.sample_count: _prev_block_size;
  process(_input, _output);
  restore_denormals(state);
  audio_check(output);
  _prev_block_size = _input.data.sample_count;
  _prev_start_perf_count = new_start_perf_count;
  return _output;
}

} // namespace inf::base