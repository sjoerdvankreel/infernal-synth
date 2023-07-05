#include <inf.synth/oscillator/graph_spectrum.hpp>

using namespace inf::base;

namespace inf::synth {
 
void 
oscillator_spectrum_graph::dsp_to_plot(graph_plot_input const& input, std::vector<float>& plot)
{
  float const* spectrum = _analyzer.analyze(input.dsp_output->data(), input.dsp_output->size(), input.sample_rate);
  for(std::size_t i = 0; i < spectrum_analyzer::bucket_count; i++)
    plot.push_back(spectrum[i]);
}

} // namespace inf::synth