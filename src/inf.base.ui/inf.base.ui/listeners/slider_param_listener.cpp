#include <inf.base.ui/listeners/slider_param_listener.hpp>

using namespace juce;
using namespace inf::base;

namespace inf::base::ui 
{

void 
slider_param_listener::controller_param_changed(param_value ui_value)
{
  auto const& data = _controller->topology()->params[_param_index].descriptor->data;
  if (data.type == param_type::real) _slider->setValue(ui_value.real, dontSendNotification);
  else _slider->setValue(ui_value.discrete, dontSendNotification);
}

void
slider_param_listener::sliderValueChanged(Slider* slider)
{
  auto const& data = _controller->topology()->params[_param_index].descriptor->data;
  if(data.type == param_type::real) _controller->editor_param_changed(_param_index, param_value(static_cast<float>(slider->getValue())));
  else _controller->editor_param_changed(_param_index, param_value(static_cast<std::int32_t>(std::round(slider->getValue()))));
}

} // namespace inf::base::ui