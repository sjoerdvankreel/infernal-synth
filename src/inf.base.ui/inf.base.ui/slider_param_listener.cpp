#include <inf.base.ui/slider_param_listener.hpp>

using namespace juce;
using namespace inf::base;

namespace inf::base::ui 
{

void 
slider_param_listener::param_changed(param_value value)
{
  auto const& data = _controller->topology()->params[_param_index].descriptor->data;
  if (data.is_continuous()) _slider->setValue(value.real);
  else _slider->setValue(value.discrete);
}

void
slider_param_listener::sliderValueChanged(Slider* slider)
{
  auto const& data = _controller->topology()->params[_param_index].descriptor->data;
  if(data.is_continuous()) _controller->editor_changed_param(_param_index, param_value(static_cast<float>(slider->getValue())));
  else _controller->editor_changed_param(_param_index, param_value(static_cast<std::int32_t>(std::round(slider->getValue()))));
}

} // namespace inf::base::ui