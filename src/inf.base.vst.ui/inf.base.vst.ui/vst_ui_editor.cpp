#include <inf.base.vst/shared/support.hpp>
#include <inf.base.vst/sdk/vst_parameter.hpp>
#include <inf.base.vst.ui/vst_ui_editor.hpp>
#include <inf.base/topology/param_ui_descriptor.hpp>

#include <cassert>
#include <algorithm>

using namespace Steinberg;
using namespace VSTGUI;
using namespace inf::base;
using namespace inf::base::ui;

namespace inf::base::vst::ui {
    
vst_ui_editor::
vst_ui_editor(vst_ui_controller* controller, UTF8StringPtr template_name,
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
  plugin_ui_editor::view_removed(view_frame, view);
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

// Force context menu open on left-click on stuff that's not a control.
// At least studio one (maybe other hosts) dont like opening context menu outside a control.
void
vst_ui_editor::onMouseEvent(MouseEvent& event, CFrame* which_frame)
{
  if (event.type != EventType::MouseDown || event.buttonState.isRight())
  {
    // Not a left-mouse event, do default behaviour.
    VST3Editor::onMouseEvent(event, which_frame);
    return;
  }

  ParamID tag;
  tresult found = find_parameter(event.mousePosition, tag);
  if(found == kResultOk)
  {
    // Clicked on a parameter, do default behaviour.
    VST3Editor::onMouseEvent(event, which_frame);
    return;
  }

  CPoint size = getFrame()->getViewSize().getSize();
  if (event.mousePosition.x <= 0.0 
  || event.mousePosition.y <= 0.0 
  || event.mousePosition.x >= size.x 
  || event.mousePosition.y >= size.y)
  {
    // Outside plugin window, do default behaviour.
    VST3Editor::onMouseEvent(event, which_frame);
    return;
  }

  // Force popup controller context menu without appending host menu items.
  SharedPointer<CFrame> frame_owner = getFrame();
  COptionMenu* menu = dynamic_cast<vst_ui_controller&>(*getController()).createContextMenu(event.mousePosition, this);
  getFrame()->doAfterEventProcessing([=, position = event.mousePosition]() {
    menu->setStyle(COptionMenu::kPopupStyle);
    menu->popup(frame_owner, position);
    menu->forget();
  });
  event.consumed = true;
}

} // namespace inf::base::vst::ui