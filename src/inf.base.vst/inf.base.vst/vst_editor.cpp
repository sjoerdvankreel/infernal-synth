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
EditorView(controller), _controller(controller)
#if __linux__
, _impl(std::make_unique<impl>())
#endif // __linux__
{ assert(controller != nullptr); }

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
vst_editor::getSize(ViewRect* new_size)
{
  if(!have_ui()) return EditorView::getSize(new_size);
  new_size->top = rect.top;
  new_size->left = rect.left;
  new_size->right = rect.left + get_ui()->getWidth();
  new_size->bottom = rect.top + get_ui()->getHeight();
  return kResultTrue;
}

tresult PLUGIN_API
vst_editor::onSize(ViewRect* new_size)
{
  if (!have_ui() || !new_size
    || (new_size->left == rect.left && new_size->right == rect.right 
    && new_size->top == rect.top && new_size->bottom == rect.bottom))
    return EditorView::onSize(new_size);
  get_ui()->removeFromDesktop();
  _controller->editor_current_width(new_size->getWidth());
  _ui = create_ui();
  _ui->build();
  _ui->layout();
  get_ui()->addToDesktop(0, systemWindow);
  return EditorView::onSize(new_size);
}

tresult PLUGIN_API
vst_editor::attached(void* parent, FIDString type)
{
  if(!plugFrame) return EditorView::attached(parent, type);
  MessageManager::getInstance();
  _controller->editor_current_width(_controller->editor_default_width());
  _ui = create_ui();
  _ui->build();
  _ui->layout();
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

tresult PLUGIN_API
vst_editor::checkSizeConstraint(ViewRect* new_size)
{
  if (!have_ui() || !new_size) return EditorView::checkSizeConstraint(new_size);
  if (new_size->getWidth() < _controller->editor_min_width())
    new_size->right = new_size->left + _controller->editor_min_width();
  double w = get_ui()->getWidth();
  double h = get_ui()->getHeight();
  new_size->bottom = new_size->top + static_cast<std::int32_t>(std::ceil(new_size->getWidth() * h / w));
  return kResultTrue;
}

} // namespace inf::base::vst