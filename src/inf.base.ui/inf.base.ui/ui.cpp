#include <inf.base.ui/ui.hpp>

using namespace juce;
using namespace inf::base;

namespace inf::base::ui {

Component* 
root_element::build_core()
{
  fill_component* result = new fill_component;
  result->fill(juce::Colours::black);
  result->addChildComponent(_content->build());
  result->setOpaque(true);
  return result;
}

Component*
ui_element::build()
{
  _component.reset(build_core());
  for (auto const& color : _colors)
    _component->setColour(color.first, color.second);
  _component->setVisible(true);
  return _component.get();
}

void
root_element::layout()
{
  std::int32_t w = _width;
  std::int32_t h = _content->pixel_height(w);
  component()->setBounds(0, 0, w, h);
  _content->component()->setBounds(0, 0, w, h);
  _content->layout();
}

Component*
param_element::build_core()
{
  Slider* result = new Slider;
  result->setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
  return result;
}

std::unique_ptr<param_element>
create_param_ui(plugin_controller* controller,
  std::int32_t part_type, std::int32_t part_index, std::int32_t param_index)
{ 
  part_id id(part_type, part_index);
  return std::make_unique<param_element>(controller, id, param_index);
}

Component*
grid_element::build_core()
{
  Component* result = new Component;
  for (std::size_t i = 0; i < _cell_contents.size(); i++)
    result->addChildComponent(_cell_contents[i]->build());
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

ui_element*
grid_element::add_cell(
  std::unique_ptr<ui_element>&& content, Rectangle<std::int32_t> const& bounds)
{
  ui_element* result = content.get();
  _cell_bounds.push_back(bounds);
  _cell_contents.push_back(std::move(content));
  return result;
}

void
grid_element::layout()
{
  Grid grid;
  for(std::int32_t row = 0; row < _size.y; row++)
    grid.templateRows.add(Grid::TrackInfo(Grid::Fr(1)));
  for (std::int32_t col = 0; col < _size.x; col++)
    grid.templateColumns.add(Grid::TrackInfo(Grid::Fr(1)));
  for (std::size_t i = 0; i < _cell_contents.size(); i++)
  {
    Rectangle<std::int32_t> bounds = _cell_bounds[i];
    GridItem item(_cell_contents[i]->component());
    item.withArea(bounds.getY(), bounds.getX(), bounds.getY() + bounds.getHeight(), bounds.getX() + bounds.getWidth());
    grid.items.add(item);
  }
  grid.performLayout(component()->getLocalBounds());
  for (std::size_t i = 0; i < _cell_contents.size(); i++)
    _cell_contents[i]->layout();
}

ui_element*
add_grid_cell(
  grid_element* grid, std::unique_ptr<ui_element>&& content,
  std::int32_t row, std::int32_t col, std::int32_t row_span, std::int32_t col_span)
{
  Rectangle<std::int32_t> bounds(col, row, col_span, row_span);
  return grid->add_cell(std::move(content), bounds);
}

ui_element*
add_grid_param_cell(grid_element* grid,
  std::int32_t part_type, std::int32_t part_index, std::int32_t param_index,
  std::int32_t row, std::int32_t col, std::int32_t row_span, std::int32_t col_span)
{
  auto param = create_param_ui(grid->controller(), part_type, part_index, param_index);
  return add_grid_cell(grid, std::move(param), row, col, row_span, col_span);
}

} // namespace inf::base::ui