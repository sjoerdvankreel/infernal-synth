#include <inf.base.vst.ui/vst_ui_editor.hpp>

using namespace Steinberg;
using namespace inf::base;

namespace inf::base::vst::ui {

vst_ui_editor::
vst_ui_editor(vst_ui_controller* controller, topology_info const* topology):
EditorView(controller)
{ 
  assert(topology != nullptr);
  assert(controller != nullptr);
}

} // namespace inf::base::vst::ui