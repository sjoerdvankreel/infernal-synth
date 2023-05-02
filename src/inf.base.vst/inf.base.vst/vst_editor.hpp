#ifndef INF_BASE_VST_VST_EDITOR_HPP
#define INF_BASE_VST_VST_EDITOR_HPP

#include <inf.base.ui/shared/ui.hpp>
#include <inf.base.ui/shared/look_and_feel.hpp>
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
  std::unique_ptr<inf::base::ui::inf_look_and_feel> _lnf = {};
  juce::Component* get_ui() const { return _ui.get()->component(); }
  bool have_ui() const { return _ui && _ui.get() && _ui.get()->component(); }

public:
  ~vst_editor() override;
  tresult PLUGIN_API removed() override;
  tresult PLUGIN_API onSize(ViewRect* new_size) override;
  tresult PLUGIN_API getSize(ViewRect* new_size) override;
  tresult PLUGIN_API attached(void* parent, FIDString type) override;
  tresult PLUGIN_API isPlatformTypeSupported(FIDString type) override;
  tresult PLUGIN_API checkSizeConstraint(ViewRect* view_rect) override;
  Steinberg::tresult PLUGIN_API canResize() override { return Steinberg::kResultTrue; }

protected:
  explicit vst_editor(vst_controller* controller);
  virtual std::unique_ptr<inf::base::ui::inf_look_and_feel> create_lnf() const = 0;
  virtual std::unique_ptr<inf::base::ui::root_element> create_ui(std::int32_t width) const = 0;
};

} // namespace inf::base::vst
#endif // INF_BASE_VST_VST_EDITOR_HPP
