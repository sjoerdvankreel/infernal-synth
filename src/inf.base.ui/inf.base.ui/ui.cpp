#include <inf.base.ui/ui.hpp>

using namespace juce;

namespace inf::base::ui {

Component* 
ui_element::render()
{
  _component.reset(render_core());
  _component->setVisible(true);
  return _component.get();
}

Component* 
root_element::render_core()
{
  Component* result = new root_component;
  result->setOpaque(true);
  result->setBounds(0, 0, _size.x, _size.y);
  Component* content = _content->render();
  content->setBounds(0, 0, _size.x, _size.y);
  result->addChildComponent(content);
  return result;
}

Component*
param_element::render_core()
{
  Slider* result = new Slider;
  result->setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
  return result;
}

Component*
grid_element::render_core()
{
  Component* result = new Component;
  return result;
}

} // namespace inf::base::ui