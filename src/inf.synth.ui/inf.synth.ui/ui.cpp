#include <inf.synth.ui/ui.hpp>

using namespace juce;
using namespace inf::base::ui;

namespace inf::synth::ui {

Component*
create_synth_ui(juce_gui_state& state)
{
  Component* result = new Component;
  result->setSize(150, 50);
  Grid grid;
  TextEditor* text1 = state.create<TextEditor>();
  text1->setSize(50, 50);
  text1->setVisible(true);
  result->addChildComponent(text1);
  TextEditor* text2 = state.create<TextEditor>();
  text2->setSize(100, 50);
  text2->setVisible(true);
  result->addChildComponent(text2);
  grid.templateRows = { Grid::TrackInfo(Grid::Fr(1)) };
  grid.templateColumns = { Grid::TrackInfo(Grid::Fr(1)), Grid::TrackInfo(Grid::Fr(2)) };
  grid.items = { juce::GridItem(text1), juce::GridItem(text2) };
  grid.performLayout(Rectangle<int>(150, 50));
  return result;
}

} // inf::synth::ui 