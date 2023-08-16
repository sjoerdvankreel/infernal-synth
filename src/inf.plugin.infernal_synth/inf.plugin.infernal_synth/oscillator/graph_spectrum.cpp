#include <inf.plugin.infernal_synth/oscillator/graph_spectrum.hpp>

using namespace inf::base;

namespace inf::plugin::infernal_synth {
 
void 
oscillator_spectrum_graph::dsp_to_plot(graph_plot_input const& input, std::vector<float>& plot)
{
  std::vector<float> const& spectrum = _analyzer.analyze(input.dsp_output->data(), input.dsp_output->size());
  for(std::size_t i = 0; i < spectrum.size(); i++)
    plot.push_back(spectrum[i]);
}

} // namespace inf::plugin::infernal_synth