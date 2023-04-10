#include <inf.base.vst/vst_editor.hpp>
#include <inf.base.vst/vst_controller.hpp>

using namespace Steinberg;

namespace inf::base::vst {

vst_editor::
vst_editor(vst_controller* controller):
EditorView(controller)
{ assert(controller != nullptr); }

} // namespace inf::base::vst