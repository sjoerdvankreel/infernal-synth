#include <inf.base.vst/shared/support.hpp>
#include <inf.base.vst/sdk/vst_parameter.hpp>
#include <inf.base.vst.ui/vst_ui_editor.hpp>
#include <inf.base/topology/param_ui_descriptor.hpp>

#include <cassert>
#include <algorithm>

using namespace VSTGUI;
using namespace inf::base;
using namespace inf::base::ui;

namespace inf::base::vst::ui {
    
vst_ui_editor::
vst_ui_editor(EditController* controller, UTF8StringPtr template_name,
  UTF8StringPtr xml_file, topology_info const* topology):
VST3Editor(controller, template_name, xml_file),
plugin_ui_editor(controller, topology)
{ 
  assert(topology != nullptr);
  assert(xml_file != nullptr);
  assert(controller != nullptr);
  assert(template_name != nullptr);
}

void 
vst_ui_editor::onViewAdded(CFrame* view_frame, CView* view)
{
  VST3Editor::onViewAdded(view_frame, view);
  plugin_ui_editor::view_added(view_frame, view);
}
 
void 
vst_ui_editor::onViewRemoved(CFrame* view_frame, CView* view)
{
  plugin_ui_editor::view_remove(view_frame, view);
  VST3Editor::onViewRemoved(view_frame, view);
}

// Set initial visibility when the plugin ui is opened.
bool PLUGIN_API
vst_ui_editor::open(void* parent, const PlatformType& type)
{
  if (!VST3Editor::open(parent, type)) return false;
  dynamic_cast<vst_ui_controller&>(*getController()).sync_ui_parameters();
  return true;
}

} // namespace inf::base::vst::ui