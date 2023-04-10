#ifndef INF_BASE_VST_VST_EDITOR_HPP
#define INF_BASE_VST_VST_EDITOR_HPP

#include <public.sdk/source/vst/vsteditcontroller.h>

namespace inf::base::vst {

class vst_controller;

class vst_editor: 
public Steinberg::Vst::EditorView
{
  using tresult = Steinberg::tresult;
  using FIDString = Steinberg::FIDString;

public:
  vst_editor(vst_controller* controller);
  tresult PLUGIN_API attached(void* parent, FIDString type) override;
  tresult PLUGIN_API isPlatformTypeSupported(FIDString type) override;
};

} // namespace inf::base::vst
#endif // INF_BASE_VST_VST_EDITOR_HPP
