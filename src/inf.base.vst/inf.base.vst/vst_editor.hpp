#ifndef INF_BASE_VST_VST_EDITOR_HPP
#define INF_BASE_VST_VST_EDITOR_HPP

#include <inf.base.ui/support.hpp>
#include <juce_gui_basics/juce_gui_basics.h>
#include <public.sdk/source/vst/vsteditcontroller.h>
#include <memory>

namespace inf::base::vst {

class vst_controller;

#if __linux__
class vst_linux_event_handler:
public Steinberg::Linux::IEventHandler,
public Steinberg::FObject
{
public:
  void PLUGIN_API onFDIsSet(Steinberg::Linux::FileDescriptor fd) override;
  DELEGATE_REFCOUNT(Steinberg::FObject)
  DEFINE_INTERFACES
  DEF_INTERFACE(Steinberg::Linux::IEventHandler)
  END_DEFINE_INTERFACES(Steinberg::FObject)
};
#endif // __linux__

class vst_editor: 
public Steinberg::Vst::EditorView
{
  using tresult = Steinberg::tresult;
  using ViewRect = Steinberg::ViewRect;
  using FIDString = Steinberg::FIDString;
  inf::base::ui::juce_gui_state _state = {};
  std::unique_ptr<juce::Component> _root = {};

protected:
  explicit vst_editor(vst_controller* controller);
  virtual juce::Component* create_content(inf::base::ui::juce_gui_state& state) = 0;

public:
#if __linux__
  int get_default_screen_fd() const;
  vst_linux_event_handler _handler = {};
  Steinberg::Linux::IRunLoop* get_run_loop() const;
#endif // __linux__

  tresult PLUGIN_API removed() override;
  tresult PLUGIN_API onSize(ViewRect* new_size) override;
  tresult PLUGIN_API attached(void* parent, FIDString type) override;
  tresult PLUGIN_API isPlatformTypeSupported(FIDString type) override;
  tresult PLUGIN_API checkSizeConstraint(ViewRect* view_rect) override;
  Steinberg::tresult PLUGIN_API canResize() override { return Steinberg::kResultFalse; }
};

} // namespace inf::base::vst
#endif // INF_BASE_VST_VST_EDITOR_HPP
