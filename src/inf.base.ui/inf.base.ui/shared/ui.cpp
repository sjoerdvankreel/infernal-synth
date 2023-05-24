#include <inf.base.ui/shared/ui.hpp>
#include <inf.base.ui/shared/config.hpp>
#include <inf.base.ui/shared/real_bounds_range.hpp>
#include <inf.base.ui/controls/label.hpp>
#include <inf.base.ui/controls/dropdown.hpp>
#include <inf.base.ui/controls/toggle_button.hpp>
#include <inf.base.ui/controls/selector_label.hpp>
#include <inf.base/shared/support.hpp>

using namespace juce;
using namespace inf::base;

namespace inf::base::ui {

void
ui_element::relevant_if(part_id id, std::int32_t param_index, bool hide_if_irrelevant, relevance_selector selector)
{
  _relevant_if_part = id;
  _relevant_if_param = param_index;
  _relevant_if_selector = selector;
  _hide_if_irrelevant = hide_if_irrelevant;
}

Component*
ui_element::build(LookAndFeel const& lnf)
{
  _component.reset(build_core(lnf));
  _component->setEnabled(true);
  _component->setVisible(_initially_visible);
  if (_relevant_if_selector != nullptr)
  {
    std::int32_t index = controller()->topology()->param_index(_relevant_if_part, _relevant_if_param);
    assert(controller()->topology()->params[index].descriptor->data.type != param_type::real);
    std::int32_t rt_part_index = controller()->topology()->params[index].part_index;
    std::int32_t part_index = _controller->topology()->parts[rt_part_index].type_index;
    param_value value = controller()->state()[index];
    if(_hide_if_irrelevant) _component->setVisible(_relevant_if_selector(part_index, value.discrete));
    else _component->setEnabled(_relevant_if_selector(part_index, value.discrete));
    _relevance_listener.reset(new relevance_listener(controller(), _component.get(), index, _hide_if_irrelevant, _relevant_if_selector));
  }
  return _component.get();
}

Component* 
root_element::build_core(LookAndFeel const& lnf)
{
  container_component* result = new container_component(
    container_component::flags::fill, 0.0f, 0.0f, 
    inf_look_and_feel::colors::root_background, inf_look_and_feel::colors::root_background, 0, 0);
  result->addChildComponent(_content->build(lnf));
  result->setOpaque(true);
  result->setLookAndFeel(&_lnf);
  _tooltip.reset(new TooltipWindow(result));
  return result;
}

void
root_element::layout()
{
  std::int32_t w = _width;
  std::int32_t h = static_cast<std::int32_t>(std::ceil(1.0f / controller()->editor_aspect_ratio() * w));
  component()->setBounds(0, 0, w, h);
  _content->component()->setBounds(0, 0, w, h);
  _content->layout();
}

Component*
container_element::build_core(LookAndFeel const& lnf)
{
  container_component* result = new container_component(
    _flags, container_radius, container_outline_size, _fill_low_color_id, 
    _fill_high_color_id, _outline_low_color_id, _outline_high_color_id);
  result->addChildComponent(_content->build(lnf));
  return result;
}

void
container_element::layout()
{
  auto bounds = component()->getLocalBounds();
  auto with_padding = Rectangle<int>(
    bounds.getX() + _padding.x / 2,
    bounds.getY() + _padding.y / 2,
    bounds.getWidth() - _padding.x,
    bounds.getHeight() - _padding.y);
  _content->component()->setBounds(with_padding);
  _content->layout();
}

Component*
group_label_element::build_core(LookAndFeel const& lnf)
{
  Label* result = new inf_label(false);
  result->setText(_text, dontSendNotification);
  result->setJustificationType(Justification::centred);
  result->setFont(juce::Font(get_group_label_font_height(controller()), juce::Font::bold));
  result->setColour(Label::ColourIds::textColourId, lnf.findColour(inf_look_and_feel::colors::part_group_label));
  return result;
}

void
group_label_element::layout()
{
  auto label = dynamic_cast<Label*>(component());
  label->setBorderSize(BorderSize<int>());
  float rotation_angles = _vertical? 270.0f / 360.0f * 2.0f * pi32: 0.0f;
  auto transform = AffineTransform().rotated(rotation_angles, label->getWidth() / 2.0f, label->getHeight() / 2.0f);
  label->setTransform(transform);
  label->setBounds(label->getBounds().transformedBy(transform));
  label->setJustificationType(_vertical? Justification::centred : Justification::centredBottom);
}

Component*
selector_label_element::build_core(LookAndFeel const& lnf)
{
  Label* result = new inf_selector_label();
  result->setText(_text, dontSendNotification);
  result->setFont(juce::Font(get_selector_font_height(controller()), juce::Font::bold));
  return result;
}

Component* 
tab_bar_element::build_core(LookAndFeel const& lnf)
{
  std::int32_t index = controller()->topology()->param_index(_part_id, _param_index);
  inf_tabbed_button_bar* result = new inf_tabbed_button_bar();
  result->setMinimumTabScaleFactor(0.0f);
  for(std::size_t i = 0; i < _headers.size(); i++)
    result->addTab(_headers[i], Colours::black, static_cast<int>(i));
  result->setCurrentTabIndex(controller()->state()[index].discrete, false);
  _listener.reset(new tab_param_listener(controller(), result, index));
  result->add_listener(_listener.get());
  if(_extra_listener) result->add_listener(_extra_listener.get());
  return result;
}

Component*
param_label_element::build_core(LookAndFeel const& lnf)
{
  Label* result = new inf_label(true);
  result->setBorderSize(BorderSize<int>());
  auto topology = controller()->topology();
  float font_height = get_param_label_font_height(controller());
  auto const& desc = topology->get_param_descriptor(_part_id, _param_index);
  std::int32_t index = controller()->topology()->param_index(_part_id, _param_index);
  param_value value = controller()->ui_value_at(_part_id, _param_index);
  if(desc.data.type == param_type::real) value.real = desc.data.real.display.to_range(value.real);
  result->setJustificationType(_justification);
  result->setFont(juce::Font(font_height, juce::Font::bold));
  result->setText(get_label_text(&desc, _type, value), dontSendNotification);
  result->setColour(Label::ColourIds::textColourId, lnf.findColour(inf_look_and_feel::colors::param_label));
  _listener.reset(new label_param_listener(controller(), result, index, _type));
  return result;
}

Component*
param_icon_element::build_core(LookAndFeel const& lnf)
{
  inf_icon* result = new inf_icon(_value, _selector);
  if(_selector != nullptr)
  {
    std::int32_t index = controller()->topology()->param_index(_part_id, _param_index);
    _listener.reset(new icon_param_listener(controller(), result, index));
  }
  return result;
}

std::unique_ptr<param_icon_element>
create_param_icon_ui(
  inf::base::plugin_controller* controller, std::int32_t part_type, 
  std::int32_t part_index, std::int32_t param_index, icon_selector selector)
{ 
  std::int32_t index = controller->topology()->param_index({ part_type, part_index }, param_index);
  std::int32_t icon_value = controller->state()[index].discrete;
  return std::make_unique<param_icon_element>(controller, part_id(part_type, part_index), param_index, icon_value, selector);
}

void 
param_edit_element::layout()
{
  auto bounds = component()->getBounds().expanded(-edit_margin, -edit_margin);
  if (_type == edit_type::dropdown)
  {
    float total_height = get_dropdown_height(controller());
    float total_width = bounds.getWidth() - dropdown_hpad;
    float drop_x = bounds.getX() + dropdown_hpad / 2.0f;
    float drop_y = bounds.getY() + (bounds.getHeight() - total_height) / 2.0f;
    bounds = Rectangle<int>(
      static_cast<int>(drop_x), static_cast<int>(drop_y),
      static_cast<int>(total_width), static_cast<int>(total_height));
  }
  component()->setBounds(bounds);
}

Component* 
param_edit_element::build_core(juce::LookAndFeel const& lnf)
{
  Component* result = nullptr;
  switch (_type)
  {
  case edit_type::toggle: result = build_toggle_core(lnf); break;
  case edit_type::dropdown: result = build_dropdown_core(lnf); break;
  default: result = build_slider_core(lnf); break;
  }
  auto client = dynamic_cast<SettableTooltipClient*>(result);
  std::int32_t index = controller()->topology()->param_index(_part_id, _param_index);
  auto const& desc = *controller()->topology()->params[index].descriptor;
  if (_tooltip_type == tooltip_type::label)
    client->setTooltip(desc.data.static_name.detail);
  else if(_tooltip_type == tooltip_type::value)
  {
    _tooltip_listener.reset(new tooltip_listener(controller(), client, index));
    param_value ui_value = controller()->ui_value_at(_part_id, _param_index);
    client->setTooltip(desc.data.format(false, ui_value) + desc.data.unit);
  }
  return result;
}

Component*
param_edit_element::build_toggle_core(LookAndFeel const& lnf)
{
  std::int32_t index = controller()->topology()->param_index(_part_id, _param_index);
  ToggleButton* result = new inf_toggle_button(_force_toggle_on);
  _toggle_listener.reset(new toggle_param_listener(controller(), result, index));
  result->setToggleState(controller()->state()[index].discrete != 0, dontSendNotification);
  result->addListener(_toggle_listener.get());
  return result;
}

Component*
param_edit_element::build_dropdown_core(LookAndFeel const& lnf)
{
  std::int32_t index = controller()->topology()->param_index(_part_id, _param_index);
  std::int32_t part_index = controller()->topology()->params[index].part_index;
  auto const& desc = controller()->topology()->get_param_descriptor(_part_id, _param_index);
  inf_dropdown* result = new inf_dropdown(&desc);
  result->setJustificationType(Justification::centred);
  if (desc.data.discrete.items != nullptr)
    for(std::int32_t i = 0; i <= desc.data.discrete.effective_max(part_index); i++)
      result->addItem((*desc.data.discrete.items)[i].name, static_cast<std::int32_t>(i) + dropdown_id_offset);
  else
    for (std::int32_t i = 0; i <= desc.data.discrete.effective_max(part_index); i++)
      result->addItem((*desc.data.discrete.names)[i], static_cast<std::int32_t>(i) + dropdown_id_offset);
  result->setSelectedItemIndex(controller()->state()[index].discrete, dontSendNotification);
  _dropdown_listener.reset(new dropdown_param_listener(controller(), result, index));
  result->addListener(_dropdown_listener.get());
  return result;
}

Component*
param_edit_element::build_slider_core(LookAndFeel const& lnf)
{
  std::int32_t index = controller()->topology()->param_index(_part_id, _param_index);
  auto const& desc = controller()->topology()->get_param_descriptor(_part_id, _param_index);
  auto default_value = controller()->topology()->base_to_ui_value(index, desc.data.default_value());
  std::int32_t part_index = controller()->topology()->params[index].part_index;
  inf_slider* result = new inf_slider(&desc, _type);
  result->setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
  if(desc.data.type == param_type::real)
  {
    result->setNormalisableRange(real_bounds_range(desc.data.real.display)),
    result->setValue(desc.data.real.display.to_range(controller()->state()[index].real), dontSendNotification);
    result->setDoubleClickReturnValue(true, default_value.real, ModifierKeys::noModifiers);
  }
  else
  {
    result->setRange(desc.data.discrete.min, desc.data.discrete.effective_max(part_index), 1.0);
    result->setValue(controller()->state()[index].discrete, dontSendNotification);
    result->setDoubleClickReturnValue(true, default_value.discrete, ModifierKeys::noModifiers);
  }
  switch (_type)
  {
  case edit_type::knob:
  case edit_type::selector:
    result->setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    break;
  case edit_type::hslider:
    result->setSliderStyle(Slider::SliderStyle::LinearHorizontal);
    break;
  case edit_type::vslider:
    result->setSliderStyle(Slider::SliderStyle::LinearVertical);
    break;
  default:
    assert(false);
    break;
  }
  _slider_listener.reset(new slider_param_listener(controller(), result, index));
  result->addListener(_slider_listener.get());
  return result;
}

juce::Component* 
part_graph_element::build_core(juce::LookAndFeel const& lnf)
{ 
  inf_graph_plot* result = new inf_graph_plot(controller(), _part_id, _graph_type); 
  _listener.reset(new graph_listener(controller(), result, _part_id, _graph_type, _tooltip_param));
  auto const& desc = controller()->topology()->get_part_descriptor(_part_id);
  result->setTooltip(desc.name_selector(controller()->topology(), controller()->state(), _part_id, _graph_type));
  return result;
}

Component*
grid_element::build_core(LookAndFeel const& lnf)
{
  Component* result = new Component;
  for (std::size_t i = 0; i < _cell_contents.size(); i++)
    result->addChildComponent(_cell_contents[i]->build(lnf));
  return result;
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
  for(std::size_t row = 0; row < _row_distribution.size(); row++)
    grid.templateRows.add(_row_distribution[row]);
  for (std::size_t col = 0; col < _column_distribution.size(); col++)
    grid.templateColumns.add(_column_distribution[col]);
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

std::unique_ptr<grid_element>
create_grid_ui(
  plugin_controller* controller,
  std::vector<std::int32_t> const& row_distribution_relative, 
  std::vector<std::int32_t> const& column_distribution_relative)
{
  std::vector<Grid::TrackInfo> row_distribution;
  std::vector<Grid::TrackInfo> column_distribution;
  for(std::size_t i = 0; i < row_distribution_relative.size(); i++)
    row_distribution.push_back(Grid::TrackInfo(Grid::Fr(row_distribution_relative[i])));
  for (std::size_t i = 0; i < column_distribution_relative.size(); i++)
    column_distribution.push_back(Grid::TrackInfo(Grid::Fr(column_distribution_relative[i])));
  return create_grid_ui(controller, row_distribution, column_distribution);
}

std::unique_ptr<ui_element>
create_param_ui(
  plugin_controller* controller, std::unique_ptr<ui_element>&& label_or_icon, std::int32_t part_type, 
  std::int32_t part_index, std::int32_t param_index, edit_type edit_type, tooltip_type tooltip_type, bool force_toggle_on, std::int32_t hslider_cols)
{
  if (edit_type == edit_type::hslider)
  {
    assert(hslider_cols != -1);
    if(hslider_cols == -1) return {};
    auto result = create_grid_ui(controller, 1, hslider_cols);
    result->add_cell(std::move(label_or_icon), 0, 0);
    result->add_cell(create_param_edit_ui(controller, part_type, part_index, param_index, edit_type, tooltip_type, force_toggle_on), 0, 1, 1, hslider_cols - 1);
    return result;
  }
  else
  {
    auto auto_rest = Grid::TrackInfo(Grid::Fr(1));
    auto fixed_label_height = Grid::TrackInfo(Grid::Px(get_param_label_total_height(controller)));
    auto result = create_grid_ui(controller, { auto_rest, fixed_label_height }, { auto_rest });
    result->add_cell(create_param_edit_ui(controller, part_type, part_index, param_index, edit_type, tooltip_type, force_toggle_on), 0, 0);
    result->add_cell(std::move(label_or_icon), 1, 0);
    return result;
  }
}

std::unique_ptr<ui_element>
create_labeled_param_ui(
  plugin_controller* controller, std::int32_t part_type, std::int32_t part_index, std::int32_t param_index, 
  edit_type edit_type, label_type label_type, tooltip_type tooltip_type, bool force_toggle_on, std::int32_t hslider_cols)
{
  auto justification = edit_type == edit_type::hslider? juce::Justification::centredRight: juce::Justification::centred;
  auto label = create_param_label_ui(controller, part_type, part_index, param_index, label_type, justification);
  return create_param_ui(controller, std::move(label), part_type, part_index, param_index, edit_type, tooltip_type, force_toggle_on, hslider_cols);
}

std::unique_ptr<ui_element>
create_iconed_param_ui(
  plugin_controller* controller, std::int32_t part_type, std::int32_t part_index, std::int32_t param_index, 
  edit_type edit_type, icon_type icon_type, tooltip_type tooltip_type, bool force_toggle_on)
{
  auto icon = create_param_icon_ui(controller, icon_type);
  return create_param_ui(controller, std::move(icon), part_type, part_index, param_index, edit_type, tooltip_type, force_toggle_on);
}

std::unique_ptr<ui_element>
create_iconed_param_ui(
  plugin_controller* controller, std::int32_t part_type, std::int32_t part_index, std::int32_t param_index, 
  edit_type edit_type, icon_selector icon_selector, tooltip_type tooltip_type, bool force_toggle_on)
{
  auto icon = create_param_icon_ui(controller, part_type, part_index, param_index, icon_selector);
  return create_param_ui(controller, std::move(icon), part_type, part_index, param_index, edit_type, tooltip_type, force_toggle_on);
}

std::unique_ptr<ui_element>
create_part_group_ui(plugin_controller* controller, std::unique_ptr<group_label_element>&& label, std::unique_ptr<ui_element>&& content)
{
  std::vector<Grid::TrackInfo> rows;
  std::vector<Grid::TrackInfo> cols;
  auto label_height = get_group_label_total_height(controller);
  if (label->vertical())
  {
    rows.push_back(Grid::TrackInfo(Grid::Fr(1)));
    cols.push_back(Grid::TrackInfo(Grid::Px(label_height)));
    cols.push_back(Grid::TrackInfo(Grid::Fr(1)));
  }
  else
  {
    rows.push_back(Grid::TrackInfo(Grid::Px(label_height)));
    rows.push_back(Grid::TrackInfo(Grid::Fr(1)));
    cols.push_back(Grid::TrackInfo(Grid::Fr(1)));
  }
  auto result = create_grid_ui(controller, rows, cols);
  if (label->vertical())
  {
    result->add_cell(std::move(label), 0, 0);
    result->add_cell(std::move(content), 0, 1);
  }
  else
  {
    result->add_cell(std::move(label), 0, 0);
    result->add_cell(std::move(content), 1, 0);
  }
  return result;
}

std::unique_ptr<ui_element>
create_part_selector_ui(
  plugin_controller* controller, std::int32_t selector_part_type, std::int32_t selector_param_index,
  std::int32_t label_columns, std::int32_t selector_columns, std::vector<std::unique_ptr<ui_element>>&& selected_parts)
{
  inf::base::part_id selector_id = { selector_part_type, 0 };
  auto const& desc = controller->topology()->get_param_descriptor(selector_id, selector_param_index);
  auto selector_grid = create_grid_ui(controller, 1, selector_columns + label_columns);
  selector_grid->add_cell(create_selector_label_ui(controller, desc.data.static_name.short_), 0, 0, 1, label_columns);
  auto tab_bar = create_tab_bar(controller, selector_id, selector_param_index);
  auto tab_bar_ptr = tab_bar.get();
  for(std::int32_t i = 0; i < static_cast<std::int32_t>(selected_parts.size()); i++)
    tab_bar->add_header(std::to_string(i + 1));
  selector_grid->add_cell(std::move(tab_bar), 0, label_columns, 1, selector_columns);

  std::vector<ui_element*> tabs;
  auto selector_height = static_cast<std::int32_t>(std::ceil(get_selector_height(controller)));
  auto result = create_grid_ui(controller, { Grid::Px(selector_height), Grid::Fr(1) }, { Grid::Fr(1) });
  result->add_cell(create_part_group_container_ui(controller, std::move(selector_grid), container_selector_padding), 0, 0);
  for(std::size_t i = 0; i < selected_parts.size(); i++)
  {
    tabs.push_back(selected_parts[i].get());
    result->add_cell(std::move(selected_parts[i]), 1, 0);
  }

  std::int32_t selected_index = controller->state()[controller->topology()->param_index(selector_id, selector_param_index)].discrete;
  for(std::int32_t i = 0; i < static_cast<std::int32_t>(selected_parts.size()); i++)
    tabs[i]->initially_visible(i == selected_index);
  tab_bar_ptr->set_extra_listener(std::make_unique<tab_button_listener>(
    std::function<void(inf_tabbed_button_bar*)>([tabs](inf_tabbed_button_bar* bar) {
      for(std::int32_t i = 0; i < static_cast<std::int32_t>(tabs.size()); i++)
        tabs[i]->component()->setVisible(i == bar->getCurrentTabIndex()); })));
  return result;
}

} // namespace inf::base::ui