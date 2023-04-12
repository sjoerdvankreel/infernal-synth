#include <inf.base.ui/ui_state.hpp>

using namespace juce;

namespace inf::base::ui {

void
ui_state::clear()
{
  _listeners.clear();
  _components.clear();
}

std::pair<juce::Slider*, juce::Label*>
ui_state::add_new_knob(Component* component, std::int32_t part, std::int32_t index, std::int32_t param)
{
  topology_info const* topology = _controller->topology();
  std::int32_t param_index = topology->param_index({ part, index }, param);
  auto const* data = &topology->params[param_index].descriptor->data;

  Slider* slider = create_component<Slider>();
  slider->setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
  slider->setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
  if(data->is_continuous()) slider->setRange(data->real.display.min, data->real.display.max, 0.0);
  else slider->setRange(data->discrete.min, data->discrete.max, 1.0);
  component->addChildComponent(slider);

  Label* label = create_component<Label>();
  label->attachToComponent(slider, false);
  label->setVisible(true);
  label->setText(juce::String(data->static_name.short_), dontSendNotification);
  component->addChildComponent(label);

  std::unique_ptr<knob_listener> listener(std::make_unique<knob_listener>(_controller, slider, param_index));
  knob_listener* listener_ptr = listener.get();
  _listeners.push_back(std::move(listener));
  slider->addListener(listener_ptr);
  
  return std::make_pair(slider, label);
}

} // namespace inf::base::ui