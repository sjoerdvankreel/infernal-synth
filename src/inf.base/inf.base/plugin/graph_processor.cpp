#include <inf.base/shared/state.hpp>
#include <inf.base/shared/support.hpp>
#include <inf.base/plugin/graph_processor.hpp>

#include <cmath>

namespace inf::base {

std::vector<graph_point> const&
graph_processor::plot(param_value const* state, 
  float sample_rate, std::int32_t width, std::int32_t height)
{
  _plot_data.clear();
  _graph_data.clear();
  process_dsp(state, sample_rate);
  if(_raw_data_storage->size() == 0) return _graph_data;
  
  graph_plot_input input;
  input.state = state;
  input.sample_rate = sample_rate;
  input.dsp_output = _raw_data_storage;
  dsp_to_plot(input, _plot_data);
  float transform = width / static_cast<float>(_plot_data.size());
  for (std::size_t i = 0; i < _plot_data.size(); i++)
    _graph_data.push_back({ static_cast<float>(i * transform), _plot_data[i] * height});
  return _graph_data;
}

// Given parameter state and sample rate, setup automation and run some dsp code.
std::vector<float> const&
graph_processor::process_dsp(param_value const* state, float sample_rate)
{
  _raw_data_storage->clear();
  _continuous_automation_buffer.clear();
  std::int32_t samples = sample_count(state, sample_rate);
  _raw_data_storage->resize(samples);
  if (samples == 0) return *_raw_data_storage;
    
  _continuous_automation_buffer.reserve(samples * topology()->continuous_param_count);
  for (std::int32_t p = 0; p < topology()->input_param_count; p++)
  {
    _state_copy[p] = transform_param(p, state[p]);
    auto const& descriptor = topology()->params[p].descriptor->data;
    if (descriptor.kind == param_kind::fixed)
      for (std::int32_t s = 0; s < samples; s++)
        _continuous_automation_buffer.push_back(descriptor.real.default_);
    else if (_topology->params[p].descriptor->data.kind == param_kind::continuous)
      for (std::int32_t s = 0; s < samples; s++)
        _continuous_automation_buffer.push_back(transform_param(p, state[p]).real);
  }

  std::int32_t continuous = 0;
  for (std::int32_t p = 0; p < topology()->input_param_count; p++)
    if (topology()->params[p].descriptor->data.is_continuous())
      _continuous_automation[p] = _continuous_automation_buffer.data() + continuous++ * samples;

  block_input input;
  input.note_count = 0;
  input.notes = nullptr;
  input.data.bpm = graph_bpm;
  input.data.stream_position = 0;
  input.data.sample_count = samples;
  input.block_automation_raw = _state_copy.data();
  input.continuous_automation_raw = _continuous_automation.data();
  input.data.automation = automation_view(
    topology(), _state_copy.data(), _continuous_automation.data(), nullptr,
    topology()->input_param_count, topology()->input_param_count, 0,
    input.data.sample_count, 0, input.data.sample_count);

  // This produces the graph specific dsp data.
  std::uint64_t denormal_state = disable_denormals();
  process_dsp_core(input, _raw_data_storage->data(), sample_rate);
  restore_denormals(denormal_state);
  return *_raw_data_storage;
}

} // namespace inf::base