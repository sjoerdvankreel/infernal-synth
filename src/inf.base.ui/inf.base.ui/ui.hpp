#ifndef INF_BASE_UI_UI_HPP
#define INF_BASE_UI_UI_HPP

#include <inf.base/plugin/plugin_controller.hpp>
#include <juce_gui_basics/juce_gui_basics.h>

#include <memory>
#include <cstdint>

namespace inf::base::ui {

class root_component:
public juce::Component
{
public:
  void paint(juce::Graphics& g) override { g.fillAll(juce::Colours::black); }
};

class ui_element
{
  plugin_controller* const _controller;
  std::unique_ptr<juce::Component> _component = {};
public:
  juce::Component* render();
  juce::Component* get() { return _component.get(); }
protected:
  virtual juce::Component* render_core() = 0;
  plugin_controller* controller() const { return _controller; }
  ui_element(plugin_controller* controller) : _controller(controller) {}
};

class cell_element:
public ui_element
{
  juce::Rectangle<std::int32_t> const _bounds; // Row/col/span bounds.
public:
  juce::Rectangle<std::int32_t> const& bounds() const { return _bounds; }
protected:
  cell_element(plugin_controller* controller, juce::Rectangle<std::int32_t> const& bounds) :
  ui_element(controller), _bounds(bounds) {}
};

class param_element:
public cell_element
{
  base::part_id const _part_id;
  std::int32_t const _param_index;
protected:
  juce::Component* render_core() override;
public:
  param_element(
    plugin_controller* controller, juce::Rectangle<std::int32_t> const& bounds, 
    base::part_id const& part_id, std::int32_t param_index):
  cell_element(controller, bounds), _part_id(part_id), _param_index(param_index) {}
};

inline std::unique_ptr<param_element>
create_param_ui(plugin_controller* controller,
  std::int32_t part_type, std::int32_t part_index, std::int32_t param_index,
  std::int32_t row, std::int32_t column, std::int32_t row_span = 1, std::int32_t column_span = 1)
{
  base::part_id id(part_type, part_index);
  juce::Rectangle<std::int32_t> bounds(column, row, column_span, row_span);
  return std::make_unique<param_element>(controller, bounds, id, param_index);
}

class grid_element:
public cell_element
{
  float const _xy_ratio;
  juce::Point<std::int32_t> const _size; // Rows/cols.
  std::vector<std::unique_ptr<ui_element>> _cells = {};
protected:
  juce::Component* render_core() override;
public:
  grid_element(
    plugin_controller* controller, juce::Rectangle<std::int32_t> const& bounds, 
    juce::Point<std::int32_t> const& size, float xy_ratio) : 
  cell_element(controller, bounds), _xy_ratio(xy_ratio), _size(size) {}
};

class root_element:
public ui_element
{
  std::int32_t const _width; // Pixel size.
  std::unique_ptr<ui_element> _content = {};
protected:
  juce::Component* render_core() override;
public:
  root_element(plugin_controller* controller, std::int32_t width):
  ui_element(controller), _width(width) {}
  void content(std::unique_ptr<ui_element>&& content) { _content = std::move(content); }
};

inline std::unique_ptr<root_element>
create_root_ui(plugin_controller* controller, std::int32_t width)
{ return std::make_unique<root_element>(controller, width); }

} // namespace inf::base::ui

#endif // INF_BASE_UI_UI_HPP