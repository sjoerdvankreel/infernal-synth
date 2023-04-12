#include <inf.base.ui/knob_listener.hpp>

namespace inf::base::ui 
{

void 
knob_listener::param_changed(inf::base::param_value value)
{
  topology_info const* topology = _controller->topology();
  auto const* data = &topology->params[_param_index].descriptor->data;
  if (!data->is_continuous()) _slider->setValue(value.discrete);
  else _slider->setValue(data->real.display.to_range(value.real));
}

void
knob_listener::sliderValueChanged(juce::Slider* slider)
{
  topology_info const* topology = _controller->topology();
  auto const* data = &topology->params[_param_index].descriptor->data;
  if(!data->is_continuous()) _controller->editor_changed_param(_param_index, param_value(static_cast<std::int32_t>(slider->getValue())));
  else _controller->editor_changed_param(_param_index, param_value(data->real.display.from_range(static_cast<float>(slider->getValue()))));
}

} // namespace inf::base::ui