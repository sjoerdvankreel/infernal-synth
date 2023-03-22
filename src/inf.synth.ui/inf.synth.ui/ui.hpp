#ifndef INF_SYNTH_UI_UI_HPP
#define INF_SYNTH_UI_UI_HPP

#include <vstgui/lib/cframe.h>

namespace inf::synth::ui {

VSTGUI::CFrame*
create_synth_ui(VSTGUI::VSTGUIEditorInterface* editor);

} // inf::synth::ui
#endif // INF_SYNTH_UI_UI_HPP