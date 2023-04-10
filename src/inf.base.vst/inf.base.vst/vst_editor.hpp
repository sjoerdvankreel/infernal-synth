#ifndef INF_BASE_VST_VST_EDITOR_HPP
#define INF_BASE_VST_VST_EDITOR_HPP

#include <juce_gui_basics/juce_gui_basics.h>
#include <public.sdk/source/vst/vsteditcontroller.h>

namespace inf::base::vst {

class vst_controller;

class vst_editor: 
public Steinberg::Vst::EditorView
{
  using tresult = Steinberg::tresult;
  using FIDString = Steinberg::FIDString;

protected:
  explicit vst_editor(vst_controller* controller);
  virtual juce::Component* create_content() = 0;

public:
  tresult PLUGIN_API attached(void* parent, FIDString type) override;
  tresult PLUGIN_API isPlatformTypeSupported(FIDString type) override;
};

} // namespace inf::base::vst
#endif // INF_BASE_VST_VST_EDITOR_HPP
