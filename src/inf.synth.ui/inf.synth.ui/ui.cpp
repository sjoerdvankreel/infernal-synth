#include <inf.base.ui/controls/rotary_knob.hpp>
#include <inf.base/topology/topology_info.hpp>

#include <vstgui/lib/cframe.h>

using namespace VSTGUI;
using namespace inf::base;
using namespace inf::base::ui;

namespace inf::synth::ui {

CFrame*
create_synth_ui(VSTGUI::VSTGUIEditorInterface* editor)
{
  CFrame* result = new CFrame(CRect(0, 0, 640, 480), editor);
  result->enableTooltips(true);
  result->setTransparency(true);
  result->setViewAddedRemovedObserver(&dynamic_cast<IViewAddedRemovedObserver&>(*editor));

  knob_ui_colors cols = {};
  cols.drag.color = 0xFFFF0000;
  cols.fill.color = 0xFFFF0000;
  auto knob = new rotary_knob(cols, true, false);
  knob->setViewSize(CRect(0, 0, 30, 30));
  result->addView(knob);
  return result;
}

} // inf::synth::ui