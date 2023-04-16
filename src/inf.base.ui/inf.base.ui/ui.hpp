#ifndef INF_BASE_UI_UI_HPP
#define INF_BASE_UI_UI_HPP

#include <inf.base/plugin/plugin_controller.hpp>
#include <juce_gui_basics/juce_gui_basics.h>

#include <map>
#include <memory>
#include <cstdint>

namespace inf::base::ui {

class fill_component:
public juce::Component
{
  juce::Colour _fill;
public:
  void fill(juce::Colour const& fill) { _fill = fill; }
  void paint(juce::Graphics& g) override { g.fillAll(_fill); }
};

class ui_element
{
  plugin_controller* const _controller;
  std::unique_ptr<juce::Component> _component = {};
  std::map<std::int32_t, juce::Colour> _colors = {};
protected:
  virtual juce::Component* build_core() = 0;
  ui_element(plugin_controller* controller) : _controller(controller) {}
public:
  juce::Component* build();
  virtual void layout() = 0;
  juce::Component* component() { return _component.get(); }
  plugin_controller* controller() const { return _controller; }
  void color(std::int32_t id, juce::Colour color) { _colors[id] = color; }
};

// Just to force a background color.
class fill_element:
public ui_element
{
  juce::Colour const _fill;
  std::unique_ptr<ui_element> _content = {};
protected:
  juce::Component* build_core() override;
public:
  void layout() override;
  void content(std::unique_ptr<ui_element>&& content) { _content = std::move(content); }
  fill_element(plugin_controller* controller, juce::Colour const& fill) : ui_element(controller), _fill(fill) {}
};

inline std::unique_ptr<fill_element>
create_fill_ui(plugin_controller* controller, juce::Colour const& fill)
{ return std::make_unique<fill_element>(controller, fill); }

class param_element:
public ui_element
{
  base::part_id const _part_id;
  std::int32_t const _param_index;
protected:
  juce::Component* build_core() override;
public:
  void layout() override {}
  param_element(
    plugin_controller* controller, base::part_id const& part_id, std::int32_t param_index):
    ui_element(controller), _part_id(part_id), _param_index(param_index) {}
};

std::unique_ptr<param_element>
create_param_ui(plugin_controller* controller,
  std::int32_t part_type, std::int32_t part_index, std::int32_t param_index);

class grid_element:
public ui_element
{
  double const _xy_ratio;
  juce::Point<std::int32_t> const _size; // Rows/cols.
  std::vector<std::unique_ptr<ui_element>> _cell_contents = {};
  std::vector<juce::Rectangle<std::int32_t>> _cell_bounds = {};
protected:
  juce::Component* build_core() override;
public:
  void layout() override;
  std::int32_t pixel_height(std::int32_t pixel_width);
  ui_element* add_cell(std::unique_ptr<ui_element>&& content, juce::Rectangle<std::int32_t> const& bounds);
  grid_element(plugin_controller* controller, juce::Point<std::int32_t> const& size, double xy_ratio) :
    ui_element(controller), _xy_ratio(xy_ratio), _size(size) {}
};

inline std::unique_ptr<grid_element>
create_grid_ui(plugin_controller* controller, std::int32_t rows, std::int32_t cols, double xy_ratio)
{ return std::make_unique<grid_element>(controller, juce::Point(cols, rows), xy_ratio); }

ui_element*
add_grid_cell(grid_element* grid, 
  std::unique_ptr<ui_element>&& content, 
  std::int32_t row, std::int32_t col, std::int32_t row_span = 1, std::int32_t col_span = 1);

ui_element*
add_grid_param_cell(grid_element* grid, 
  std::int32_t part_type, std::int32_t part_index, std::int32_t param_index,
  std::int32_t row, std::int32_t col, std::int32_t row_span = 1, std::int32_t col_span = 1);

class root_element:
public ui_element
{
  juce::Colour const _fill;
  std::int32_t const _width; // Pixel size.
  std::unique_ptr<grid_element> _content = {};
protected:
  juce::Component* build_core() override;
public:
  void layout() override;
  void content(std::unique_ptr<grid_element>&& content) { _content = std::move(content); }
  root_element(plugin_controller* controller, std::int32_t width, juce::Colour const& fill) :
  ui_element(controller), _fill(fill), _width(width) {}
};

inline std::unique_ptr<root_element>
create_root_ui(plugin_controller* controller, std::int32_t width, juce::Colour const& fill)
{ return std::make_unique<root_element>(controller, width, fill); }

} // namespace inf::base::ui

#endif // INF_BASE_UI_UI_HPP