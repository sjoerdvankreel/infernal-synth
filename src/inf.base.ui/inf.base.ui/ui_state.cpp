#include <inf.base.ui/ui_state.hpp>

using namespace juce;

namespace inf::base::ui {

void
ui_state::clear()
{
  _components.clear();
  _listeners.clear();
}

Slider*
ui_state::create_knob(std::int32_t part, std::int32_t index, std::int32_t param)
{
  topology_info const* topology = _controller->topology();
  std::int32_t param_index = topology->param_index({ part, index }, param);
  auto const* data = &topology->params[param_index].descriptor->data;

  Slider* result = create_component<Slider>();
  result->setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
  if(data->is_continuous()) result->setRange(data->real.display.min, data->real.display.max, 0.0);
  else result->setRange(data->discrete.min, data->discrete.max, 1.0);

  std::unique_ptr<slider_listener> listener(std::make_unique<slider_listener>(_controller, param_index));
  slider_listener* listener_ptr = listener.get();
  _listeners.push_back(std::move(listener));
  result->addListener(listener_ptr);
  return result;
}

} // namespace inf::base::ui