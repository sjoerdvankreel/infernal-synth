#include <inf.base.vst/vst_editor.hpp>
#include <inf.base.vst/vst_controller.hpp>

#include <cstring>

using namespace juce;
using namespace Steinberg;

namespace inf::base::vst {

vst_editor::
vst_editor(vst_controller* controller):
EditorView(controller)
{ assert(controller != nullptr); }

tresult PLUGIN_API 
vst_editor::removed()
{
  if (_content)
  {
    _content->removeFromDesktop();
    _content.reset();
  }
  return EditorView::removed();
}

tresult PLUGIN_API 
vst_editor::attached(void* parent, FIDString type)
{
  _content.reset(create_content());
  _content->setOpaque(true);
  _content->addToDesktop(0, (void*)parent);
  _content->setVisible(true);
  ViewRect vr(0, 0, _content->getWidth(), _content->getHeight());
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