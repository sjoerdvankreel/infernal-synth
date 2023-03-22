#include <inf.base.vst/shared/support.hpp>
#include <inf.base.vst/sdk/vst_parameter.hpp>
#include <inf.base.vst.ui/vst_ui_editor.hpp>
#include <inf.base/topology/param_ui_descriptor.hpp>
#include <inf.base.ui/controls/rotary_knob.hpp>

#include <cassert>
#include <algorithm>

using namespace VSTGUI;
using namespace inf::base;
using namespace inf::base::ui;

namespace inf::base::vst::ui {
    
vst_ui_editor::
vst_ui_editor(vst_ui_controller* controller, topology_info const* topology):
plugin_ui_editor(controller, topology),
VSTGUIEditor(controller)
{ 
  assert(topology != nullptr);
  assert(controller != nullptr);
}

void PLUGIN_API
vst_ui_editor::close()
{
  //Steinberg::IdleUpdateHandler::stop();
  if (getFrame() == nullptr) return;
  getFrame()->removeAll(true);
  if (getFrame()->getNbReference() > 1)
  {
    getFrame()->forget();
    return;
  }
  getFrame()->close();
}

bool PLUGIN_API 
vst_ui_editor::open(void* parent, const PlatformType& type)
{
  frame = new CFrame(CRect(0, 0, 640, 480), this);
  knob_ui_colors cols = {};
  cols.drag.color = 0xFFFF0000;
  cols.fill.color = 0xFFFF0000;
  auto knob = new inf::base::ui::rotary_knob(cols, true, false);
  knob->setViewSize(CRect(0, 0, 30, 30));
  frame->addView(knob);
  getFrame()->enableTooltips(true);
  getFrame()->setTransparency(true);
  getFrame()->setViewAddedRemovedObserver(this);
  getFrame()->open(parent, type, nullptr);
  //Steinberg::IdleUpdateHandler::start();
  dynamic_cast<vst_ui_controller&>(*getController()).sync_ui_parameters();
  return true;
}

} // namespace inf::base::vst::ui