#ifndef INF_SYNTH_UI_UI_HPP
#define INF_SYNTH_UI_UI_HPP

#include <inf.base.ui/ui_state.hpp>
#include <inf.base.ui/root_component.hpp>
#include <juce_gui_basics/juce_gui_basics.h>

namespace inf::synth::ui {

inf::base::ui::root_component*
create_synth_ui(inf::base::ui::ui_state& state);

} // inf::synth::ui 
#endif // INF_SYNTH_UI_UI_HPP