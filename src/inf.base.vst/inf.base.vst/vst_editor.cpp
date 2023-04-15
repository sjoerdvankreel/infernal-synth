#if __linux__
#define INF_INCLUDE_JUCE_VST3_WRAPPER 1
#include "juce_VST3_Wrapper_modified.cpp"
#endif // __linux__

#include <inf.base.vst/vst_editor.hpp>
#include <inf.base.vst/vst_controller.hpp>
#include <cstring>

using namespace juce;
using namespace inf::base::ui;
using namespace Steinberg;

namespace inf::base::vst {

#if __linux__
struct vst_editor::impl
{
  juce::SharedResourcePointer<juce::EventHandler> event_handler;
  juce::SharedResourcePointer<juce::MessageThread> message_thread;
};
#endif // __linux__

vst_editor::
~vst_editor() {}

vst_editor::
vst_editor(vst_controller* controller):
EditorView(controller)
#if __linux__
, _impl(std::make_unique<impl>())
#endif // __linux__
{ assert(controller != nullptr); }

tresult PLUGIN_API
vst_editor::checkSizeConstraint(ViewRect* new_rect)
{
  if (have_ui()) return kResultFalse;
  int cw = _ui.get()->get()->getWidth();
  int ch = _ui.get()->get()->getHeight();
  new_rect->right = new_rect->left + cw;
  new_rect->bottom = new_rect->top + ch;
  return Steinberg::kResultTrue;
}

tresult PLUGIN_API
vst_editor::onSize(ViewRect* new_size)
{
  if (have_ui()) return EditorView::onSize(new_size);
  int cw = get_ui()->getWidth();
  int ch = get_ui()->getHeight();
  int cx = get_ui()->getScreenX();
  int cy = get_ui()->getScreenY();
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
  if (have_ui()) get_ui()->removeFromDesktop();
  _ui.reset();
#if __linux__
  _impl->event_handler->unregisterHandlerForFrame(plugFrame);
#endif // __linux__
  return EditorView::removed();
}

tresult PLUGIN_API
vst_editor::attached(void* parent, FIDString type)
{
  assert(plugFrame);
  MessageManager::getInstance();
  _ui.reset(create_ui(juce::Point<std::int32_t>(rect.getWidth(), rect.getHeight())));
#if __linux__
  _impl->event_handler->registerHandlerForFrame(plugFrame);
#endif // __linux__
  get_ui()->addToDesktop(0, (void*)parent);
  ViewRect vr(0, 0, get_ui()->getWidth(), get_ui()->getHeight());
  setRect(vr);
  plugFrame->resizeView(this, &vr);
  return EditorView::attached(parent, type);
}

tresult PLUGIN_API
vst_editor::isPlatformTypeSupported(FIDString type)
{
  if (std::strcmp(type, kPlatformTypeHWND) == 0) return kResultTrue;
  if (std::strcmp(type, kPlatformTypeX11EmbedWindowID) == 0) return kResultTrue;
  return kResultFalse;
}

} // namespace inf::base::vst