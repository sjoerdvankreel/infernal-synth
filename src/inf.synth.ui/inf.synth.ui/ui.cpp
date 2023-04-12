#include <inf.synth.ui/ui.hpp>
#include <inf.synth/synth/topology.hpp>
#include <inf.synth/oscillator/topology.hpp>
#include <inf.synth/audio_bank/topology.hpp>

using namespace juce;
using namespace inf::base;
using namespace inf::base::ui;

namespace inf::synth::ui {

root_component*
create_synth_ui(ui_state& state)
{
  std::int32_t w = 800;
  std::int32_t h = 600;
  root_component* result = new root_component;
  result->setSize(w, h);
  auto slider = state.add_new_knob(result, part_type::vaudio_bank, 0, audio_bank_param::bal1);
  slider.first->setSize(50, 50);
  slider.second->setTopLeftPosition(0, 50);
  slider.second->setSize(50, 10);
  return result;
  /*
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
  */
}

} // inf::synth::ui 