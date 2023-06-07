#include <inf.base.ui/listeners/last_edit_value_param_listener.hpp>

using namespace juce;
using namespace inf::base;

namespace inf::base::ui {

void
last_edit_value_param_listener::textEditorTextChanged(juce::TextEditor&)
{
  param_value ui_value;
  auto text = _editor->getText().toStdString();
  auto const& param_info = _controller->topology()->params[_last_param_index];
  if(!param_info.descriptor->data.parse(false, param_info.part_index, text.c_str(), ui_value)) return;
  _controller->editor_param_changed(_last_param_index, ui_value);
}

void 
last_edit_value_param_listener::any_controller_param_changed(std::int32_t index)
{
  if(_last_param_index == index) return;
  auto value = _controller->ui_value_at_index(index);
  auto const& param_info = _controller->topology()->params[index];
  auto const& part_desc = *_controller->topology()->parts[param_info.part_index].descriptor;
  if (part_desc.kind != part_kind::input) return;
  _editor->setText(_controller->topology()->params[index].descriptor->data.format(false, value));
  _last_param_index = index;
}

} // namespace inf::base::ui