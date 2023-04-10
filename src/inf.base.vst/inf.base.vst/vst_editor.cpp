#include <inf.base.vst/vst_editor.hpp>
#include <inf.base.vst/vst_controller.hpp>

#include <cstring>

using namespace Steinberg;

namespace inf::base::vst {

vst_editor::
vst_editor(vst_controller* controller):
EditorView(controller)
{ assert(controller != nullptr); }

tresult PLUGIN_API 
vst_editor::attached(void* parent, FIDString type)
{
  ViewRect vr(0, 0, 100, 100);
  setRect(vr);
  if(plugFrame) plugFrame->resizeView(this, &vr);
  return EditorView::attached(parent, type);
}

tresult PLUGIN_API 
vst_editor::isPlatformTypeSupported(FIDString type)
{
  if(std::strcmp(type, kPlatformTypeHWND) == 0) return kResultTrue;
  if(std::strcmp(type, kPlatformTypeX11EmbedWindowID) == 0) return kResultTrue;
  return kResultFalse;
}

} // namespace inf::base::vst