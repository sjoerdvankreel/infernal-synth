#ifndef INF_BASE_VST_VST_EDITOR_HPP
#define INF_BASE_VST_VST_EDITOR_HPP

#include <inf.base.ui/shared/ui.hpp>
#include <juce_gui_basics/juce_gui_basics.h>
#include <public.sdk/source/vst/vsteditcontroller.h>
#include <memory>

namespace inf::base::vst {

class vst_controller;

class vst_editor: 
public Steinberg::Vst::EditorView
{
  vst_controller* const _controller;

#if __linux__
  struct impl;
  std::unique_ptr<impl> _impl;
#endif // __linux__

  using tresult = Steinberg::tresult;
  using ViewRect = Steinberg::ViewRect;
  using FIDString = Steinberg::FIDString;

  std::unique_ptr<inf::base::ui::root_element> _ui = {};
  juce::Component* get_ui() const { return _ui.get()->component(); }
  bool have_ui() const { return _ui && _ui.get() && _ui.get()->component(); }

protected:
  explicit vst_editor(vst_controller* controller);
  virtual std::unique_ptr<inf::base::ui::root_element> create_ui() = 0;

public:
  ~vst_editor() override;
  tresult PLUGIN_API removed() override;
  tresult PLUGIN_API onSize(ViewRect* new_size) override;
  tresult PLUGIN_API getSize(ViewRect* new_size) override;
  tresult PLUGIN_API attached(void* parent, FIDString type) override;
  tresult PLUGIN_API isPlatformTypeSupported(FIDString type) override;
  tresult PLUGIN_API checkSizeConstraint(ViewRect* view_rect) override;

  Steinberg::tresult PLUGIN_API canResize() override { return Steinberg::kResultTrue; }
  void* current_window() const { return _ui.get() == nullptr? nullptr: _ui.get()->component(); }
};

} // namespace inf::base::vst
#endif // INF_BASE_VST_VST_EDITOR_HPP
