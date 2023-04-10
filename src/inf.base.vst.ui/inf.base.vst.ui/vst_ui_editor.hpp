#ifndef INF_BASE_VST_UI_VST_UI_EDITOR_HPP
#define INF_BASE_VST_UI_VST_UI_EDITOR_HPP

#include <inf.base.vst.ui/vst_ui_controller.hpp>
#include <public.sdk/source/vst/vsteditcontroller.h>

namespace inf::base::vst::ui {

class vst_ui_controller;

class vst_ui_editor: 
public Steinberg::Vst::EditorView
{
public:
  vst_ui_editor(vst_ui_controller* controller, inf::base::topology_info const* topology);
};

} // namespace inf::base::vst::ui
#endif // INF_BASE_VST_UI_VST_UI_EDITOR_HPP
