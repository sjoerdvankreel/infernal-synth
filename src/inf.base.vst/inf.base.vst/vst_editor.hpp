#ifndef INF_BASE_VST_VST_EDITOR_HPP
#define INF_BASE_VST_VST_EDITOR_HPP

#include <inf.base.ui/shared/ui.hpp>
#include <juce_gui_basics/juce_gui_basics.h>
#include <public.sdk/source/vst/vsteditcontroller.h>
#include <memory>

namespace inf::base::vst {

class vst_controller;

// Just for visual debugging.
class wrapper_component :
public juce::Component
{
public:
  void paint(juce::Graphics& g) override { g.fillAll(juce::Colours::red); }
};

class vst_editor: 
public Steinberg::Vst::EditorView
{
  using tresult = Steinberg::tresult;
  using ViewRect = Steinberg::ViewRect;
  using FIDString = Steinberg::FIDString;

  vst_controller* const _controller;
  std::unique_ptr<wrapper_component> _wrapper_ui = {};
  std::unique_ptr<inf::base::ui::root_element> _plugin_ui = {};

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

  void set_width(std::int32_t width);
  Steinberg::tresult PLUGIN_API canResize() override { return Steinberg::kResultFalse; }
  void* current_window() const { return _wrapper_ui.get() == nullptr? nullptr: _wrapper_ui.get(); }
};

} // namespace inf::base::vst
#endif // INF_BASE_VST_VST_EDITOR_HPP
