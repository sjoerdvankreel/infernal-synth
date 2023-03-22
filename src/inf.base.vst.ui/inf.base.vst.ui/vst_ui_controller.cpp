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
vst_ui_controller(std::unique_ptr<inf::base::topology_info>&& topology) :
vst_controller(std::move(topology)) {} 

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

IPlugView* PLUGIN_API
vst_ui_controller::createView(char const* name)
{
  if (ConstString(name) != ViewType::kEditor) return nullptr;
  setKnobMode(KnobModes::kLinearMode);
  return create_editor();
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
// host context menu and cannot augment it otherwise some hosts
// freak out (bitwig, fruity).
COptionMenu* 
vst_ui_controller::createContextMenu(CPoint const& pos, VST3Editor* editor)
{
#if 0
  ParamID tag;
  auto& inf_editor = dynamic_cast<vst_ui_editor&>(*editor);
  tresult found = inf_editor.find_parameter(pos, tag);
  if(found == kResultOk) return VST3EditorDelegate::createContextMenu(pos, editor);
#endif

  // Init, clear, copy/swap items, and preset selector. 
  // Just do it manually, kIsProgramChange don't work well.
  auto result = new COptionMenu();
  std::vector<CMenuItem*> context_menu(create_context_menu(this));
  for(auto it = context_menu.begin(); it != context_menu.end(); ++it)
    result->addEntry(*it);
  return result;
}

} // namespace inf::base::vst::ui