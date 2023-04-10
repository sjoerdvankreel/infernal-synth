#ifndef INF_BASE_VST_VST_EDITOR_HPP
#define INF_BASE_VST_VST_EDITOR_HPP

#include <public.sdk/source/vst/vsteditcontroller.h>

namespace inf::base::vst {

class vst_controller;

class vst_editor: 
public Steinberg::Vst::EditorView
{
public:
  vst_editor(vst_controller* controller);
};

} // namespace inf::base::vst
#endif // INF_BASE_VST_VST_EDITOR_HPP
