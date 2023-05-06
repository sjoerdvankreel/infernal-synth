#include <inf.base.ui/controls/icon.hpp>

using namespace juce;

namespace inf::base::ui {

void 
inf_icon::paint(Graphics& g)
{
  g.setColour(Colours::red);
  g.fillRect(getLocalBounds());
}

} // namespace inf::base::ui