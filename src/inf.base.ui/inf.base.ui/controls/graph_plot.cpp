#include <inf.base.ui/controls/graph_plot.hpp>

using namespace juce;

namespace inf::base::ui {

void 
graph_plot::paint(juce::Graphics& g)
{
  g.setColour(Colours::rebeccapurple);
  g.fillRect(getBounds());
}

} // namespace inf::base::ui