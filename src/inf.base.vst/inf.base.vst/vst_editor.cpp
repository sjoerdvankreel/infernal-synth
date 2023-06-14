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
  if (_wrapper_ui) _wrapper_ui->removeFromDesktop();
  _plugin_ui.reset();
  _wrapper_ui.reset();
#if __linux__
  _impl->event_handler->unregisterHandlerForFrame(plugFrame);
#endif // __linux__
  return EditorView::removed();
}

tresult PLUGIN_API
vst_editor::getSize(ViewRect* new_size)
{
  if(!_wrapper_ui) return EditorView::getSize(new_size);
  new_size->top = rect.top;
  new_size->left = rect.left;
  new_size->right = rect.left + _wrapper_ui->getWidth();
  new_size->bottom = rect.top + _wrapper_ui->getHeight();
  return kResultTrue;
}

tresult PLUGIN_API
vst_editor::attached(void* parent, FIDString type)
{
  if (!plugFrame) return EditorView::attached(parent, type);
  MessageManager::getInstance();
#if __linux__
  _impl->event_handler->registerHandlerForFrame(plugFrame);
#endif // __linux__
  _controller->editor_current_width(_controller->editor_min_width());
  _plugin_ui = create_ui();
  _plugin_ui->build();
  _plugin_ui->layout();
  _wrapper_ui.reset(new Component);
  _wrapper_ui->setOpaque(true);
  _wrapper_ui->addToDesktop(0, (void*)parent);
  _wrapper_ui->setVisible(true);
  _wrapper_ui->setSize(_plugin_ui->component()->getWidth(), _plugin_ui->component()->getHeight());
  _wrapper_ui->addChildComponent(*_plugin_ui->component());
  ViewRect vr(0, 0, _wrapper_ui->getWidth(), _wrapper_ui->getHeight());
  setRect(vr);
  plugFrame->resizeView(this, &vr);
  return EditorView::attached(parent, type);
}

tresult PLUGIN_API
vst_editor::onSize(ViewRect* new_size)
{
  if (!_wrapper_ui || !new_size
    || (new_size->left == rect.left && new_size->right == rect.right 
    && new_size->top == rect.top && new_size->bottom == rect.bottom))
    return EditorView::onSize(new_size);
  _wrapper_ui->removeAllChildren();
  _wrapper_ui->setSize(rect.getWidth(), rect.getHeight());
  _controller->editor_current_width(new_size->getWidth());
  _plugin_ui = create_ui();
  _plugin_ui->build();
  _plugin_ui->layout();
  _wrapper_ui->addChildComponent(*_plugin_ui->component());
  return EditorView::onSize(new_size);
}

tresult PLUGIN_API
vst_editor::isPlatformTypeSupported(FIDString type)
{
  if (std::strcmp(type, kPlatformTypeHWND) == 0) return kResultTrue;
  if (std::strcmp(type, kPlatformTypeX11EmbedWindowID) == 0) return kResultTrue;
  return kResultFalse;
}

void
vst_editor::set_width(std::int32_t width)
{
  if (!plugFrame) return;
  auto new_rect = rect;
  new_rect.right = rect.left + width;
  new_rect.bottom = rect.top + static_cast<std::int32_t>(width / _controller->editor_aspect_ratio());
  plugFrame->resizeView(this, &new_rect);
  onSize(&new_rect);
  _controller->editor_current_width(width);
}

} // namespace inf::base::vst