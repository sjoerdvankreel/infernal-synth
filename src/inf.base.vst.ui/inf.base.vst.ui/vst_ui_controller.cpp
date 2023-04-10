#include <inf.base.vst.ui/vst_ui_editor.hpp>
#include <inf.base.vst.ui/vst_ui_controller.hpp>
   
using namespace inf::base;
using namespace Steinberg;
using namespace Steinberg::Vst;                  

namespace inf::base::vst::ui {

vst_ui_controller::
vst_ui_controller(std::unique_ptr<inf::base::topology_info>&& topology, FUID const& processor_id) :
vst_controller(std::move(topology), processor_id) {}

IPlugView* PLUGIN_API
vst_ui_controller::createView(char const* name)
{
  if (ConstString(name) != ViewType::kEditor) return nullptr;
  vst_ui_editor* result = new vst_ui_editor(this, _topology.get());
  setKnobMode(KnobModes::kLinearMode);
  return result;
}

} // namespace inf::base::vst::ui