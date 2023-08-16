#ifndef INF_BASE_FORMAT_VST3_VST_EDITOR_HPP
#define INF_BASE_FORMAT_VST3_VST_EDITOR_HPP

#include <inf.base.ui/shared/ui.hpp>
#include <juce_gui_basics/juce_gui_basics.h>
#include <public.sdk/source/vst/vsteditcontroller.h>
#include <memory>

namespace inf::base::format::vst3 {

class vst_controller;

class vst_editor: 
public Steinberg::Vst::EditorView
{
  using tresult = Steinberg::tresult;
  using ViewRect = Steinberg::ViewRect;
  using FIDString = Steinberg::FIDString;

  vst_controller* const _controller;
  std::unique_ptr<inf::base::ui::root_element> _plugin_ui = {};

  void recreate_ui(std::int32_t width, void* parent);

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
  void* current_window() const { return _plugin_ui? _plugin_ui->component(): nullptr ; }
};

} // namespace inf::base::format::vst3
#endif // INF_BASE_FORMAT_VST_VST_EDITOR_HPP
