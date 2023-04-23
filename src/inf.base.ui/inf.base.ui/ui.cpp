#include <inf.base.ui/ui.hpp>
#include <inf.base.ui/slider.hpp>

using namespace juce;
using namespace inf::base;

namespace inf::base::ui {

void 
container_component::paint(Graphics& g)
{
  if (_flags & flags::fill)
  {
    g.setColour(_fill);
    g.fillRoundedRectangle(getLocalBounds().toFloat(), _radius);
  }
  if (_flags & flags::outline)
  {
    g.setColour(_outline);
    g.drawRoundedRectangle(getLocalBounds().toFloat(), _radius, _thickness);
  }
  if(getChildren().size() != 0)
    getChildComponent(0)->paint(g);
}

Component*
ui_element::build(plugin_controller* controller)
{
  _component.reset(build_core(controller));
  for (auto const& color : _colors)
    _component->setColour(color.first, color.second);
  _component->setVisible(true);
  return _component.get();
}

Component* 
root_element::build_core(plugin_controller* controller)
{
  container_component* result = new container_component(container_component::flags::fill, 0.0f, 0.0f, Colours::black, Colour());
  result->addChildComponent(_content->build(controller));
  result->setOpaque(true);
  result->setLookAndFeel(&_lnf);
  return result;
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
container_element::build_core(plugin_controller* controller)
{
  container_component* result = new container_component(_flags, _radius, _thickness, _fill, _outline);
  result->addChildComponent(_content->build(controller));
  return result;
}

void
container_element::layout()
{
  _content->component()->setBounds(component()->getLocalBounds());
  _content->layout();
}

Component*
param_label_element::build_core(plugin_controller* controller)
{
  Label* result = new Label;
  auto const& desc = controller->topology()->get_param_descriptor(_part_id, _param_index);
  result->setFont(Font(11.0f));
  result->setJustificationType(Justification::centredTop);
  result->setText(desc.data.static_name.short_, dontSendNotification);
  return result;
}

Component*
param_slider_element::build_core(plugin_controller* controller)
{
  std::int32_t index = controller->topology()->param_index(_part_id, _param_index);
  auto const& desc = controller->topology()->get_param_descriptor(_part_id, _param_index);
  inf_slider* result = new inf_slider;
  if(desc.data.is_continuous())
  {
    result->setRange(desc.data.real.display.min, desc.data.real.display.max, 0.0);
    result->setValue(desc.data.real.display.to_range(static_cast<float>(controller->state()[index].real)), dontSendNotification);
  }
  else
  {
    result->setRange(desc.data.discrete.min, desc.data.discrete.max, 1.0);
    result->setValue(controller->state()[index].discrete, dontSendNotification);
  }
  result->bipolar(desc.data.is_continuous()? desc.data.real.display.min < 0.0f: desc.data.discrete.min < 0);
  result->setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
  result->setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
  _listener.reset(new slider_param_listener(controller, result, index));
  result->addListener(_listener.get());
  return result;
}

Component*
grid_element::build_core(plugin_controller* controller)
{
  Component* result = new Component;
  for (std::size_t i = 0; i < _cell_contents.size(); i++)
    result->addChildComponent(_cell_contents[i]->build(controller));
  return result;
}

std::int32_t 
grid_element::pixel_height(std::int32_t pixel_width)
{
  double rows = static_cast<double>(_row_distribution.size());
  double cols = static_cast<double>(_column_distribution.size());
  return static_cast<std::int32_t>(std::ceil(pixel_width * rows / cols));
}

ui_element*
grid_element::add_cell(std::unique_ptr<ui_element>&& content,
  std::int32_t row, std::int32_t col, std::int32_t row_span, std::int32_t col_span)
{
  ui_element* result = content.get();
  _cell_bounds.push_back(Rectangle<std::int32_t>(col, row, col_span, row_span));
  _cell_contents.push_back(std::move(content));
  return result;
}

void
grid_element::layout()
{
  Grid grid;
  grid.rowGap = Grid::Px(_gap_size);
  grid.columnGap = Grid::Px(_gap_size);
  for(std::size_t row = 0; row < _row_distribution.size(); row++)
    grid.templateRows.add(Grid::TrackInfo(Grid::Fr(_row_distribution[row])));
  for (std::size_t col = 0; col < _column_distribution.size(); col++)
    grid.templateColumns.add(Grid::TrackInfo(Grid::Fr(_column_distribution[col])));
  for (std::size_t i = 0; i < _cell_contents.size(); i++)
  {
    GridItem item(_cell_contents[i]->component());
    Rectangle<std::int32_t> bounds = _cell_bounds[i];
    grid.items.add(item.withArea(bounds.getY() + 1, bounds.getX() + 1, GridItem::Span(bounds.getHeight()), GridItem::Span(bounds.getWidth())));
  }
  grid.performLayout(component()->getLocalBounds());
  for (std::size_t i = 0; i < _cell_contents.size(); i++)
    _cell_contents[i]->layout();
}

std::unique_ptr<ui_element>
create_param_ui(std::int32_t part_type, std::int32_t part_index, std::int32_t param_index)
{
  auto result = create_grid_ui({ 9, 4 }, { 1 });
  result->add_cell(create_param_slider_ui(part_type, part_index, param_index), 0, 0);
  result->add_cell(create_param_label_ui(part_type, part_index, param_index), 1, 0);
  return result;
}

} // namespace inf::base::ui