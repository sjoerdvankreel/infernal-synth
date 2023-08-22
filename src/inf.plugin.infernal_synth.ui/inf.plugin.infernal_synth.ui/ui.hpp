#ifndef INF_PLUGIN_INFERNAL_SYNTH_UI_UI_HPP
#define INF_PLUGIN_INFERNAL_SYNTH_UI_UI_HPP

#include <inf.base/plugin/plugin_controller.hpp>
#include <inf.base.ui/shared/ui.hpp>

#include <juce_gui_basics/juce_gui_basics.h>
#include <memory>

namespace inf::plugin::infernal_synth::ui {

editor_properties
get_synth_editor_properties();
std::unique_ptr<inf::base::ui::root_element>
create_synth_ui(inf::base::plugin_controller* controller);

} // inf::plugin::infernal_synth::ui 
#endif // INF_PLUGIN_INFERNAL_SYNTH_UI_UI_HPP