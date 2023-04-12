#include <inf.base.ui/slider_listener.hpp>

namespace inf::base::ui 
{

void
slider_listener::sliderValueChanged(juce::Slider* slider)
{
  param_value base_value;
  topology_info const* topology = _controller->topology();
  auto const* data = &topology->params[_param_index].descriptor->data;
  if(!data->is_continuous()) _controller->edit_param(_param_index, param_value(static_cast<std::int32_t>(slider->getValue())));
  else _controller->edit_param(_param_index, param_value(data->real.display.from_range(static_cast<float>(slider->getValue()))));
}

} // namespace inf::base::ui