#include <inf.synth/shared/support.hpp>
#include <inf.synth/effect/state.hpp>
#include <inf.synth/effect/graph1.hpp>
#include <inf.synth/effect/topology.hpp>
#include <inf.synth/effect/processor.hpp>
#include <inf.synth/cv_bank/processor.hpp>

#include <memory>

using namespace inf::base;

namespace inf::synth {

static inline float constexpr reverb_length_sec = 5.0f;
static inline float constexpr delay_fdbk_length_secs = 5.0f;
static inline float constexpr delay_reverb_graph_rate = 500.0f;
static inline float constexpr delay_reverb_graph_input_freq = 100.0f;
static inline std::int32_t constexpr shaper_sample_count = 500;

base::param_value
effect_graph1::transform_param(std::int32_t rt_index, param_value value) const
{
  std::int32_t oversampling_index = topology()->param_index(id(), effect_param::shp_over_order);
  if(rt_index == oversampling_index) 
    return param_value(effect_shp_over_order::over_none);
  return graph_disable_modulation(topology(), rt_index, value);
}

float
effect_graph1::prepare_input_filter(float sample_rate)
{
  // Impulse response.
  _audio_in[0][0] = 1.0f;
  _audio_in[1][0] = 1.0f;
  return sample_rate;
}

float
effect_graph1::prepare_input_shaper(float sample_rate)
{
  // Graph1 plots shaper response to linear ramp.
  for (std::int32_t i = 0; i < shaper_sample_count; i++)
  {
    float sample = i / static_cast<float>(shaper_sample_count) * 2.0f - 1.0f;
    _audio_in[0][i] = sample;
    _audio_in[1][i] = sample;
  }
  return sample_rate;
}

float
effect_graph1::opacity(param_value const* state) const
{
  automation_view automation(topology(), state, id());
  std::int32_t type = automation.block_discrete(effect_param::type);
  std::int32_t filter_type = automation.block_discrete(effect_param::filter_type);
  switch (type)
  {
  case effect_type::delay: return 0.5f;
  case effect_type::reverb: return 0.33f;
  case effect_type::shaper: return 1.0f;
  case effect_type::filter: return filter_type == effect_filter_type::comb? 0.75f: 1.0f;
  default: assert(false); return 0.0f;
  }
}

bool
effect_graph1::needs_repaint(std::int32_t runtime_param) const
{
  std::int32_t begin = topology()->param_start(id());
  return begin <= runtime_param && runtime_param < begin + effect_param::count;
}

void
effect_graph1::dsp_to_plot(graph_plot_input const& input, std::vector<float>& plot)
{
  // For plot 1, output is already what we want for all fx types.
  // Just scale to unipolar.
  for (std::size_t s = 0; s < input.dsp_output->size(); s++)
    plot.push_back(std::clamp(((*input.dsp_output)[s] + 1.0f) * 0.5f, 0.0f, 1.0f));
}

void
effect_graph1::process_dsp_core(block_input const& input, float* output, float sample_rate)
{
  float* output_lr[2];
  process_dsp_lr(input, sample_rate, output_lr);
  std::copy(output_lr[0], output_lr[0] + input.data.sample_count, output);
}

std::int32_t
effect_graph1::sample_count_filter(param_value const* state, float sample_rate) const
{
  float const comb_length_ms = 50.0f;
  float const state_var_length_ms = 5.0f;
  float fcomb_count = std::ceil(comb_length_ms * sample_rate / 1000.0f);
  float fstvar_count = std::ceil(state_var_length_ms * sample_rate / 1000.0f);
  automation_view automation(topology(), state, id());
  std::int32_t filter_type = automation.block_discrete(effect_param::filter_type);
  switch (filter_type)
  {
  case effect_filter_type::comb: return static_cast<std::int32_t>(fcomb_count);
  case effect_filter_type::state_var: return static_cast<std::int32_t>(fstvar_count);
  default: assert(false); return -1;
  }
}

std::int32_t
effect_graph1::sample_count_delay(param_value const* state, float sample_rate) const
{
#if 0
  assert(id().type == part_type::geffect);
  automation_view automation(topology(), state, id());
  auto fx_state = std::make_unique<effect_state>(true, sample_rate, 1);
  std::int32_t delay_type = automation.block_discrete(effect_param::delay_type);
  std::int32_t tap_count = automation.block_discrete(effect_param::dly_multi_taps);
  float ffeedback_count = delay_reverb_graph_rate * delay_fdbk_length_secs;
  auto processor = std::make_unique<effect_processor>(topology(), id(),
    delay_reverb_graph_rate, graph_bpm, midi_note_c4, fx_state.get(), automation);
  switch (delay_type)
  {
  case effect_delay_type::feedback: return static_cast<std::int32_t>(ffeedback_count);
  case effect_delay_type::multi: return (tap_count + 1) * processor->graph_min_delay_samples();
  default: assert(false); return -1;
  }
#endif
  return (int)(13* delay_reverb_graph_rate);
}

std::int32_t
effect_graph1::sample_count(param_value const* state, float sample_rate) const
{
  automation_view automation(topology(), state, id());
  std::int32_t type = automation.block_discrete(effect_param::type);
  switch (type)
  {
  case effect_type::shaper: return shaper_sample_count;
  case effect_type::delay: return sample_count_delay(state, sample_rate);
  case effect_type::filter: return sample_count_filter(state, sample_rate);
  case effect_type::reverb: return static_cast<std::int32_t>(reverb_length_sec * delay_reverb_graph_rate);
  default: assert(false); return -1;
  }
}
 
float
effect_graph1::prepare_input_delay_reverb(block_input const& input, float rate)
{
  assert(id().type == part_type::geffect);
  automation_view automation = input.data.automation.rearrange_params(id());
  auto fx_state = std::make_unique<effect_state>(true, rate, input.data.sample_count);
  auto processor = std::make_unique<effect_processor>(topology(), id(), 
    delay_reverb_graph_rate, graph_bpm, midi_note_c4, fx_state.get(), automation);

  // Short fade-out sinewave, looks better in graph than single impulse.
  float phase = 0.0f;
  std::int32_t length = processor->graph_min_delay_samples() / 2;
  for (std::int32_t i = 0; i < length; i++)
  {
    float sample = std::sin(phase * 2.0f * pi32) * (length - i) / static_cast<float>(length);
    phase += delay_reverb_graph_input_freq / delay_reverb_graph_rate;
    _audio_in[0][i] = sample;
    _audio_in[1][i] = sample;
  }
  return delay_reverb_graph_rate;
}

void
effect_graph1::process_dsp_lr(base::block_input const& input, float sample_rate, float* output_lr[2])
{
  _audio_in[0].clear();
  _audio_in[1].clear();
  _audio_in[0].resize(input.data.sample_count);
  _audio_in[1].resize(input.data.sample_count);
  _audio_out[0].resize(input.data.sample_count);
  _audio_out[1].resize(input.data.sample_count);

  automation_view automation = input.data.automation.rearrange_params(id());
  std::int32_t type = automation.block_discrete(effect_param::type);
  switch (type)
  {
  case effect_type::shaper: 
    sample_rate = prepare_input_shaper(sample_rate); break;
  case effect_type::filter: 
    sample_rate = prepare_input_filter(sample_rate); break;
  case effect_type::delay: 
  case effect_type::reverb: 
    sample_rate = prepare_input_delay_reverb(input, sample_rate); break;
  default: assert(false); break;
  }

  process_graph(input, sample_rate);
  output_lr[0] = _audio_out[0].data();
  output_lr[1] = _audio_out[1].data();
}

} // namespace inf::synth