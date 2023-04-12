#include <inf.base.vst/vst_editor.hpp>
#include <inf.base.vst/vst_controller.hpp>
#include <inf.base.ui/support.hpp>
#include <cstring>

using namespace juce;
using namespace inf::base::ui;
using namespace Steinberg;

namespace juce
{
  void* create_listener();

  void delete_listener(void* l);

  void register_listener(void* l, IPlugFrame* f);

  void unregister_listener(void* l, IPlugFrame* f);

}

namespace inf::base::vst {

vst_editor::
vst_editor(vst_controller* controller):
EditorView(controller)
{ 
assert(controller != nullptr); 
#if __linux__
_l = juce::create_listener();
#endif // __linux__
}

tresult PLUGIN_API
vst_editor::checkSizeConstraint(ViewRect* new_rect)
{
  if (!_root) return kResultFalse;
  int cw = _root->getWidth();
  int ch = _root->getHeight();
  new_rect->right = new_rect->left + cw;
  new_rect->bottom = new_rect->top + ch;
  return Steinberg::kResultTrue;
}

tresult PLUGIN_API
vst_editor::onSize(ViewRect* new_size)
{
  if (!_root) return EditorView::onSize(new_size);
  int cw = _root->getWidth();
  int ch = _root->getHeight();
  int cx = _root->getScreenX();
  int cy = _root->getScreenY();
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
vst_editor::removed()
{
  if (_root)
  {
    _root->removeFromDesktop();
    _root.reset();
  }
  _state.clear();
#if __linux__
  juce::unregister_listener(_l, plugFrame);
#endif // __linux__
  return EditorView::removed();
}

tresult PLUGIN_API
vst_editor::attached(void* parent, FIDString type)
{
  assert(plugFrame);
  _state.clear();
  _root.reset(create_content(_state));
#if __linux__
  juce::unregister_listener(_l, plugFrame);
#endif // __linux__
  _root->setOpaque(true);
  _root->addToDesktop(0, (void*)parent);
  _root->setVisible(true);
  ViewRect vr(0, 0, _root->getWidth(), _root->getHeight());
  setRect(vr);
  plugFrame->resizeView(this, &vr);
  return EditorView::attached(parent, type);
}

tresult PLUGIN_API
vst_editor::isPlatformTypeSupported(FIDString type)
{
#if WIN32
  if (std::strcmp(type, kPlatformTypeHWND) == 0) return kResultTrue;
#elif __linux__
  if (std::strcmp(type, kPlatformTypeX11EmbedWindowID) == 0) return kResultTrue;
#else
#error
#endif
  return kResultFalse;
}

} // namespace inf::base::vst