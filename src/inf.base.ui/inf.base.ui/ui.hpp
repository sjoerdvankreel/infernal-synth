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
  juce::Component* get() { return _component.get(); }
  juce::Component* render(juce::Point<std::int32_t> const& size);
protected:
  plugin_controller* controller() const { return _controller; }
  ui_element(plugin_controller* controller) : _controller(controller) {}
  virtual juce::Component* render_core(juce::Point<std::int32_t> const& size) = 0;
};

class param_element:
public ui_element
{
  base::part_id const _part_id;
  std::int32_t const _param_index;
protected:
  juce::Component* render_core(juce::Point<std::int32_t> const& size) override;
public:
  param_element(
    plugin_controller* controller, base::part_id const& part_id, std::int32_t param_index):
    ui_element(controller), _part_id(part_id), _param_index(param_index) {}
};

std::unique_ptr<param_element>
create_param_ui(plugin_controller* controller,
  std::int32_t part_type, std::int32_t part_index, std::int32_t param_index,
  std::int32_t row, std::int32_t column, std::int32_t row_span = 1, std::int32_t column_span = 1);

class grid_element:
public ui_element
{
  float const _xy_ratio;
  juce::Point<std::int32_t> const _size; // Rows/cols.
  std::vector<std::unique_ptr<ui_element>> _cell_contents = {};
  std::vector<juce::Rectangle<std::int32_t>> _cell_bounds = {};
protected:
  juce::Component* render_core(juce::Point<std::int32_t> const& size) override;
public:
  std::int32_t pixel_height(std::int32_t pixel_width);
  grid_element(plugin_controller* controller, juce::Point<std::int32_t> const& size, float xy_ratio) : 
  ui_element(controller), _xy_ratio(xy_ratio), _size(size) {}
  void add_cell(juce::Rectangle<std::int32_t> const& bounds, std::unique_ptr<ui_element>&& content);
};

std::unique_ptr<grid_element>
create_grid_ui(plugin_controller* controller, std::int32_t rows, std::int32_t cols, float xy_ratio);

class root_element:
public ui_element
{
  std::int32_t const _width; // Pixel size.
  std::unique_ptr<grid_element> _content = {};
protected:
  juce::Component* render_core(juce::Point<std::int32_t> const&) override;
public:
  root_element(plugin_controller* controller, std::int32_t width):
  ui_element(controller), _width(width) {}
  void content(std::unique_ptr<grid_element>&& content) { _content = std::move(content); }
};

inline std::unique_ptr<root_element>
create_root_ui(plugin_controller* controller, std::int32_t width)
{ return std::make_unique<root_element>(controller, width); }

} // namespace inf::base::ui

#endif // INF_BASE_UI_UI_HPP