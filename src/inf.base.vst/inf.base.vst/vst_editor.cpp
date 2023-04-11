#include <inf.base.vst/vst_editor.hpp>
#include <inf.base.vst/vst_controller.hpp>
#include <inf.base.ui/support.hpp>
#include <cstring>

#include <iostream>

using namespace juce;
using namespace inf::base::ui;
using namespace Steinberg;

namespace inf::base::vst {

vst_editor::
vst_editor(vst_controller* controller):
EditorView(controller)
{ assert(controller != nullptr); }

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
  std::cout << "unregit\n";
  get_run_loop()->unregisterEventHandler(&_handler);
  std::cout << "dununregit\n";
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
  std::cout << "regit\n";
  auto fd = get_default_screen_fd();
  get_run_loop()->registerEventHandler(&_handler, fd);
  std::cout << "dunregit\n";
  std::cout << "for fd " << fd << "\n";
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

#if __linux__
void PLUGIN_API 
vst_linux_event_handler::onFDIsSet(Steinberg::Linux::FileDescriptor fd)
{
  std::cout << "get me an f-in fd0\n";
  juce::LinuxEventLoopInternal::invokeEventLoopCallbackForFd(fd);
  std::cout << "get me an f-in fd1\n";
}

int
vst_editor::get_default_screen_fd() const 
{ 
  auto display = juce::XWindowSystem::getInstance()->getDisplay();
  return juce::X11Symbols::getInstance()->xConnectionNumber(display); 
}

Steinberg::Linux::IRunLoop*
vst_editor::get_run_loop() const
{
  assert(plugFrame);
  auto iid = Steinberg::Linux::IRunLoop::iid;
  Steinberg::Linux::IRunLoop* result = nullptr;
  tresult ok = plugFrame->queryInterface(iid, reinterpret_cast<void**>(&result));
  assert(ok == kResultOk);
  (void)ok;
  std::cout << "rl = " << result << "\n";
  return result;
}
#endif // __linux__

} // namespace inf::base::vst