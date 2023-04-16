#ifndef INF_SYNTH_UI_UI_HPP
#define INF_SYNTH_UI_UI_HPP

#include <inf.base/plugin/plugin_controller.hpp>
#include <inf.base.ui/ui.hpp>

#include <juce_gui_basics/juce_gui_basics.h>
#include <memory>

namespace inf::synth::ui {

std::unique_ptr<inf::base::ui::root_element>
create_synth_ui(std::int32_t width);

} // inf::synth::ui 
#endif // INF_SYNTH_UI_UI_HPP