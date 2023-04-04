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
  // will delete itself when the frame will be destroyed
  getFrame()->registerKeyboardHook(new vst_ui_editor_keyboard_hook(this));
  dynamic_cast<vst_ui_controller&>(*getController()).sync_ui_parameters();
  return true;
}

void
vst_ui_editor::onKeyboardEvent(KeyboardEvent& event, CFrame* which_frame)
{
  // Pick an unlikely keyboard combo so as not to interfere
  // with the host. At least reaper, bitwig and fruity react
  // to various combinations of (ctrl/shift/alt/ctrl+shift/alt+shift/ctrl+alt - m).
  // Even worse, reaper also reacts to those if the plugin
  // window is in the foreground so we don't even get a chance
  // to capture the event. So we go with ctrl+alt+shift+m, 
  // since this is only a workaround for hosts which do not get the menu's right.
  if(event.character != 'm') return;

  Modifiers modifiers;
  modifiers.add(ModifierKey::Alt);
  modifiers.add(ModifierKey::Shift);
  modifiers.add(ModifierKey::Control);
  if(event.modifiers != modifiers) return;
  
  // Try to popup at mouse, otherwise some default location.
  CPoint mouse;
  CPoint size = getFrame()->getViewSize().getSize();
  CPoint defaultMouse = CPoint(size.x / 3.0, size.y / 3.0);
  if(!getFrame()->getCurrentMouseLocation(mouse)) mouse = defaultMouse;
  if (mouse.x <= 0.0 || mouse.y <= 0.0 || mouse.x >= size.x || mouse.y >= size.y) mouse = defaultMouse;

  // Force popup controller context menu without appending host menu items.
  SharedPointer<CFrame> frame_owner = getFrame();
  COptionMenu* menu = dynamic_cast<vst_ui_controller&>(*getController()).createContextMenu(mouse, this);
  getFrame()->doAfterEventProcessing([=, position = mouse]() {
    menu->setStyle(COptionMenu::kPopupStyle);
    menu->popup(frame_owner, position);
    menu->forget();
  });
  event.consumed = true;
}

} // namespace inf::base::vst::ui