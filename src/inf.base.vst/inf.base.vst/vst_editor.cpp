#include <inf.base.vst/vst_editor.hpp>
#include <inf.base.vst/vst_controller.hpp>
#include <cstring>

using namespace juce;
using namespace inf::base::ui;
using namespace Steinberg;

namespace inf::base::vst {

vst_editor::
~vst_editor() {}

vst_editor::
vst_editor(vst_controller* controller):
EditorView(controller), _controller(controller)
{ assert(controller != nullptr); }

tresult PLUGIN_API
vst_editor::removed()
{
  if(_plugin_ui && _plugin_ui->component()) _plugin_ui->component()->removeFromDesktop();
  _plugin_ui.reset();
  return EditorView::removed();
}

tresult PLUGIN_API
vst_editor::getSize(ViewRect* new_size)
{
  if(!_plugin_ui || !_plugin_ui->component()) return EditorView::getSize(new_size);
  new_size->top = rect.top;
  new_size->left = rect.left;
  new_size->right = rect.left + _plugin_ui->component()->getWidth();
  new_size->bottom = rect.top + _plugin_ui->component()->getHeight();
  return kResultTrue;
}

tresult PLUGIN_API
vst_editor::attached(void* parent, FIDString type)
{
  if (!plugFrame) return EditorView::attached(parent, type);
  MessageManager::getInstance();
  auto ui_size_names = _controller->ui_size_names();
  auto found = std::find(ui_size_names.begin(), ui_size_names.end(), _controller->get_ui_size());
  if(found == ui_size_names.end())
    recreate_ui(_controller->editor_min_width(), parent);
  else
    recreate_ui(plugin_editor_width(_controller, static_cast<std::int32_t>(found - ui_size_names.begin())), parent);
  ViewRect vr(0, 0, _plugin_ui->component()->getWidth(), _plugin_ui->component()->getHeight());
  setRect(vr);
  plugFrame->resizeView(this, &vr);
  return EditorView::attached(parent, type);
}

tresult PLUGIN_API
vst_editor::onSize(ViewRect* new_size)
{
  if (!_plugin_ui || !_plugin_ui->component() || !new_size
    || (new_size->left == rect.left && new_size->right == rect.right 
    && new_size->top == rect.top && new_size->bottom == rect.bottom))
    return EditorView::onSize(new_size);
  recreate_ui(new_size->getWidth(), systemWindow);
  return EditorView::onSize(new_size);
}

void 
vst_editor::recreate_ui(std::int32_t width, void* parent)
{
  if(_plugin_ui && _plugin_ui->component())
  {
    _plugin_ui->component()->setLookAndFeel(nullptr);
    _plugin_ui->component()->removeFromDesktop();
    _plugin_ui.reset();
  }
  _controller->editor_current_width(width);
  _plugin_ui = create_ui();
  _plugin_ui->build();
  _plugin_ui->layout();
  _plugin_ui->component()->setOpaque(true);
  _plugin_ui->component()->addToDesktop(0, parent);
  _plugin_ui->component()->setVisible(true);
}

tresult PLUGIN_API
vst_editor::isPlatformTypeSupported(FIDString type)
{
  if (std::strcmp(type, kPlatformTypeHWND) == 0) return kResultTrue;
  if (std::strcmp(type, kPlatformTypeX11EmbedWindowID) == 0) return kResultTrue;
  return kResultFalse;
}

void
vst_editor::set_width(std::int32_t width)
{
  if (!plugFrame) return;
  auto new_rect = rect;
  if(width > 0)
  {
    new_rect.right = rect.left + width;
    new_rect.bottom = rect.top + static_cast<std::int32_t>(width / _controller->editor_aspect_ratio());
  }
  plugFrame->resizeView(this, &new_rect);
  onSize(&new_rect);
  if(width > 0) _controller->editor_current_width(width);
}

} // namespace inf::base::vst