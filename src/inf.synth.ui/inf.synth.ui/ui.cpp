#include <inf.synth.ui/ui.hpp>

using namespace juce;

namespace inf::synth::ui {

juce::Component*
create_synth_ui()
{
  Component* result = new TextEditor;
  result->setSize(50, 50);
  return result;
}

} // inf::synth::ui 