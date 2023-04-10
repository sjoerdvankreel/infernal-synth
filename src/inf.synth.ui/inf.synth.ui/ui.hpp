#ifndef INF_SYNTH_UI_UI_HPP
#define INF_SYNTH_UI_UI_HPP

#include <inf.base.ui/support.hpp>
#include <juce_gui_basics/juce_gui_basics.h>

namespace inf::synth::ui {

juce::Component*
create_synth_ui(inf::base::ui::juce_gui_state& state);

} // inf::synth::ui 
#endif // INF_SYNTH_UI_UI_HPP