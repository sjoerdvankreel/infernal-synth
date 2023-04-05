#include <inf.base.ui/shared/support.hpp>
#include <inf.base.vst.ui/vst_ui_editor.hpp>
#include <inf.base.vst.ui/vst_ui_controller.hpp>
   
using namespace VSTGUI;
using namespace inf::base;
using namespace inf::base::ui;
using namespace Steinberg;
using namespace Steinberg::Vst;                  

namespace inf::base::vst::ui {

vst_ui_controller::
vst_ui_controller(std::unique_ptr<inf::base::topology_info>&& topology, FUID const& processor_id) :
vst_controller(std::move(topology), processor_id) {}

void
vst_ui_controller::view_removed(IPlugView* editor)
{
  assert(editor != nullptr);
  assert(_editor != nullptr);
  _editor = nullptr;
}

void
vst_ui_controller::view_attached(IPlugView* editor)
{
  assert(editor != nullptr);
  assert(_editor == nullptr);
  _editor = editor;
  sync_ui_parameters();
}

tresult
vst_ui_controller::endEdit(ParamID tag)
{
  update_state(tag);
  if (_editor == nullptr) return EditControllerEx1::endEdit(tag);

  // Update visibility of dependent parameters and rerender graphs.
  dynamic_cast<vst_ui_editor&>(*_editor).update_dependent_visibility(tag);
  return EditControllerEx1::endEdit(tag);
}

// Add copy/switch functionality and load-preset to context menu. 
// Unless user clicked an actual control, then we MUST popup the
// host context menu and cannot augment it otherwise FL studio crashes.
COptionMenu* 
vst_ui_controller::createContextMenu(CPoint const& pos, VST3Editor* editor)
{
  if(dynamic_cast<vst_ui_editor&>(*editor).find_parameter(pos))
    return VST3EditorDelegate::createContextMenu(pos, editor);

  // Init, clear, copy/swap items, and preset selector. 
  // Just do it manually, kIsProgramChange don't work well.
  auto result = new COptionMenu();
  auto ui_editor = &dynamic_cast<vst_ui_editor&>(*editor);
  std::vector<CMenuItem*> context_menu(create_context_menu(ui_editor));
  for(auto it = context_menu.begin(); it != context_menu.end(); ++it)
    result->addEntry(*it);
  return result;
}

IPlugView* PLUGIN_API
vst_ui_controller::createView(char const* name)
{
  if (ConstString(name) != ViewType::kEditor) return nullptr;
  vst_ui_editor* result = new vst_ui_editor(this, "view", "UI/controller.uidesc", _topology.get());
  setKnobMode(KnobModes::kLinearMode);
  return result;
}

} // namespace inf::base::vst::ui