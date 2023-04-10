#ifndef INF_BASE_VST_VST_EDITOR_HPP
#define INF_BASE_VST_VST_EDITOR_HPP

#include <juce_gui_basics/juce_gui_basics.h>
#include <public.sdk/source/vst/vsteditcontroller.h>
#include <memory>

namespace inf::base::vst {

class vst_controller;

class vst_editor: 
public Steinberg::Vst::EditorView
{
  using tresult = Steinberg::tresult;
  using ViewRect = Steinberg::ViewRect;
  using FIDString = Steinberg::FIDString;
  std::unique_ptr<juce::Component> _content = nullptr;

protected:
  explicit vst_editor(vst_controller* controller);
  virtual juce::Component* create_content() = 0;

public:
  tresult PLUGIN_API removed() override;
  tresult PLUGIN_API onSize(ViewRect* new_size) override;
  tresult PLUGIN_API attached(void* parent, FIDString type) override;
  tresult PLUGIN_API isPlatformTypeSupported(FIDString type) override;
  tresult PLUGIN_API checkSizeConstraint(ViewRect* view_rect) override;
  Steinberg::tresult PLUGIN_API canResize() override { return Steinberg::kResultFalse; }
};

} // namespace inf::base::vst
#endif // INF_BASE_VST_VST_EDITOR_HPP
