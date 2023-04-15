#include <inf.base.ui/ui.hpp>

using namespace juce;
using namespace inf::base;

namespace inf::base::ui {

Component* 
ui_element::render(juce::Point<std::int32_t> const& size)
{
  _component.reset(render_core(size));
  _component->setVisible(true);
  return _component.get();
}

Component* 
root_element::render_core(juce::Point<std::int32_t> const&)
{
  std::int32_t w = _width;
  std::int32_t h = _content->pixel_height(w);
  Component* result = new root_component;
  result->setOpaque(true);
  result->setBounds(0, 0, w, h);
  Component* content = _content->render();
  content->setBounds(0, 0, w, h);
  result->addChildComponent(content);
  return result;
}

Component*
param_element::render_core(juce::Point<std::int32_t> const& size)
{
  Slider* result = new Slider;
  result->setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
  return result;
}

std::unique_ptr<param_element>
create_param_ui(plugin_controller* controller,
  std::int32_t part_type, std::int32_t part_index, std::int32_t param_index,
  std::int32_t row, std::int32_t column, std::int32_t row_span = 1, std::int32_t column_span = 1)
{
  part_id id(part_type, part_index);
  Rectangle<std::int32_t> bounds(column, row, column_span, row_span);
  return std::make_unique<param_element>(controller, bounds, id, param_index);
}

Component*
grid_element::render_core(juce::Point<std::int32_t> const& size)
{
  Component* result = new Component;

  return result;
}

std::int32_t 
grid_element::pixel_height(std::int32_t pixel_width)
{
  double rows = _size.y;
  double cols = _size.x;
  double ratio = rows / cols * _xy_ratio;
  return static_cast<std::int32_t>(std::ceil(pixel_width * ratio));
}

void 
grid_element::add_cell(Rectangle<std::int32_t> const& bounds, std::unique_ptr<ui_element>&& content)
{
  _cell_bounds.push_back(bounds);
  _cell_contents.push_back(std::move(content));
}

std::unique_ptr<grid_element>
create_grid_ui(plugin_controller* controller, std::int32_t rows, std::int32_t cols, float xy_ratio)
{
  Point size(rows, cols);
  return std::make_unique<grid_element>(controller, size, xy_ratio);
}

} // namespace inf::base::ui