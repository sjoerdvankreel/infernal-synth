#ifndef INF_BASE_VST_UI_VST_UI_CONTROLLER_HPP
#define INF_BASE_VST_UI_VST_UI_CONTROLLER_HPP

#include <inf.base.vst/sdk/vst_controller.hpp>
#include <inf.base/topology/topology_info.hpp>

#include <pluginterfaces/vst/vsttypes.h>
#include <public.sdk/source/vst/vsteditcontroller.h>

namespace inf::base::vst::ui {

// Vst edit controller dynamically generated from topology_info.
class vst_ui_controller: 
public inf::base::vst::vst_controller
{
  using IPlugView = Steinberg::IPlugView; 

public:
  explicit vst_ui_controller(std::unique_ptr<inf::base::topology_info>&& topology, FUID const& processor_id);
  IPlugView* PLUGIN_API createView(char const* name) override;
};

} // namespace inf::base::vst::ui
#endif // INF_BASE_VST_UI_VST_UI_CONTROLLER_HPP