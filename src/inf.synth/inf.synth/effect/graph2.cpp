#include <inf.synth/shared/support.hpp>
#include <inf.synth/effect/state.hpp>
#include <inf.synth/effect/graph2.hpp>
#include <inf.synth/effect/topology.hpp>
#include <inf.synth/effect/processor.hpp>
#include <inf.synth/cv_bank/processor.hpp>

#include <memory>

using namespace inf::base;

namespace inf::synth {

std::int32_t
effect_graph2::sample_count(param_value const* state, float sample_rate) const
{ 
  // For shaper, graph2 is frequency spectrum of 1 second of shaper output over c4 sinewave.
  automation_view automation(topology(), state, id());
  switch (automation.block_discrete(effect_param::type))
  {
  case effect_type::delay:
  case effect_type::reverb:
  case effect_type::filter:
    return _graph1.sample_count(state, sample_rate);
  case effect_type::shaper:
    return static_cast<std::int32_t>(std::ceil(sample_rate / spectrum_freq_resolution));
  default:
    assert(false);
    return -1;
  }
}

// For reverb and delay, select right, graph1 is left.
// For filter, select left, they are equal, dsp_to_plot will do spectrum analysis.
// For shaper, we need to generate a new series, then transform to spectrum in dsp_to_plot.
// (Shaper plot 1 is shaper response to linear ramp, plot 2 is spectrum of shaper response to sine).
void
effect_graph2::process_dsp_core(block_input const& input, float* output, float sample_rate)
{
  float* output_lr[2];
  automation_view automation = input.data.automation.rearrange_params(id());
  switch (automation.block_discrete(effect_param::type))
  {
  case effect_type::filter:
    _graph1.process_dsp_core(input, output, sample_rate);
    return;
  case effect_type::delay:
  case effect_type::reverb:
    _graph1.process_dsp_lr(input, sample_rate, output_lr);
    std::copy(output_lr[1], output_lr[1] + input.data.sample_count, output);
    return;
  case effect_type::shaper:
    process_shaper_spectrum(input, output, sample_rate);
    return;
  default:
    assert(false);
  }
}

void 
effect_graph2::process_shaper_spectrum(block_input const& input, float* output, float sample_rate)
{
  // Shaper graph2 is spectrum of response to sine wave, set up input.
  _audio_in[0].resize(input.data.sample_count);
  _audio_in[1].resize(input.data.sample_count);
  _audio_out[0].resize(input.data.sample_count);
  _audio_out[1].resize(input.data.sample_count);

  float phase = 0.0f;
  float freq = note_to_frequency_table(midi_note_c4);
  for (std::int32_t i = 0; i < input.data.sample_count; i++)
  {
    phase += freq / sample_rate;
    phase -= std::floor(phase);
    float sample = std::sin(phase * 2.0f * pi32);
    _audio_in[0][i] = sample;
    _audio_in[1][i] = sample;
  }

  // Apply shaper.
  process_graph(input, sample_rate);
  std::copy(_audio_out[0].data(), _audio_out[0].data() + input.data.sample_count, output);
}

bool 
effect_graph2::bars(base::param_value const* state) const
{
  automation_view automation(topology(), state, id());
  std::int32_t type = automation.block_discrete(effect_param::type);
  return type == effect_type::shaper;
}

bool 
effect_graph2::bipolar(base::param_value const* state) const
{
  automation_view automation(topology(), state, id());
  std::int32_t type = automation.block_discrete(effect_param::type);
  return type == effect_type::delay || type == effect_type::reverb;
}

// https://dsp.stackexchange.com/questions/20500/negative-values-of-the-fft
void
effect_graph2::dsp_to_plot(graph_plot_input const& input, std::vector<float>& plot)
{
  automation_view automation(topology(), input.state, id());
  std::int32_t type = automation.block_discrete(effect_param::type);

  // Delay and reverb plot left/right, re-use dsp1.
  if (type == effect_type::delay || type == effect_type::reverb)
  {
    _graph1.dsp_to_plot(input, plot);
    return;
  }

  // Shaper graph2 plots spectrum of 1 second shaper output of sinewave input.
  if (type == effect_type::shaper)
  {
    std::vector<float> const& spectrum = _analyzer.analyze(*input.dsp_output);
    for (std::size_t i = 0; i < spectrum.size(); i++)
      plot.push_back(spectrum[i]);
    return;
  }

  // Filter plots IR to FR.
  assert(type == effect_type::filter);

  float max = 0.0f;
  std::vector<std::complex<float>> const& fft = _fft.transform(*input.dsp_output);
  for (std::size_t i = 0; i < fft.size(); i++)
    max = std::max(max, std::abs(fft[i].real()));
  for (std::size_t i = 0; i < fft.size(); i++)
    plot.push_back(std::abs(fft[i].real()) / max);
}

} // namespace inf::synth