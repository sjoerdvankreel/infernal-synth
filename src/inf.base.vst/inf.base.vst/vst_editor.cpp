#include <inf.base.vst/vst_editor.hpp>
#include <inf.base.vst/vst_controller.hpp>
#include <inf.base.ui/support.hpp>

#include <cstring>

using namespace juce;
using namespace inf::base::ui;
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
    recursive_destroy_children(_content.get());
    _content.reset();
  }
  return EditorView::removed();
}

tresult PLUGIN_API
vst_editor::checkSizeConstraint(ViewRect* new_rect)
{
  if (!_content) return kResultFalse;
  int cw = _content->getWidth();
  int ch = _content->getHeight();
  new_rect->right = new_rect->left + cw;
  new_rect->bottom = new_rect->top + ch;
  return Steinberg::kResultTrue;
}

tresult PLUGIN_API 
vst_editor::onSize(ViewRect* new_size)
{
  if (!_content) return EditorView::onSize(new_size);
  int cw = _content->getWidth();
  int ch = _content->getHeight();
  int cx = _content->getScreenX();
  int cy = _content->getScreenY();
  int32 ny = new_size->top;
  int32 nx = new_size->left;
  int32 nw = new_size->getWidth();
  int32 nh = new_size->getHeight();
  if (cx != nx || cy != ny || cw != nw || ch != nh) 
    return EditorView::onSize(new_size);
  EditorView::onSize(new_size);
  return Steinberg::kResultTrue;
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
  if (plugFrame) plugFrame->resizeView(this, &vr);
  return EditorView::attached(parent, type);
}

tresult PLUGIN_API
vst_editor::isPlatformTypeSupported(FIDString type)
{
#if WIN32
  if (std::strcmp(type, kPlatformTypeHWND) == 0) return kResultTrue;
#else
  if (std::strcmp(type, kPlatformTypeX11EmbedWindowID) == 0) return kResultTrue;
#endif
  return kResultFalse;
}

} // namespace inf::base::vst