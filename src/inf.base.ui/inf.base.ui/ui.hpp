#ifndef INF_BASE_UI_UI_HPP
#define INF_BASE_UI_UI_HPP

#include <inf.base/plugin/plugin_controller.hpp>
#include <juce_gui_basics/juce_gui_basics.h>

#include <map>
#include <memory>
#include <cstdint>

namespace inf::base::ui {

class container_component:
public juce::Component
{
  juce::Colour _fill;
public:
  void fill(juce::Colour const& fill) { _fill = fill; }
  void paint(juce::Graphics& g) override { g.fillAll(_fill); }
};

class ui_element
{
  std::unique_ptr<juce::Component> _component = {};
  std::map<std::int32_t, juce::Colour> _colors = {};
protected:
  virtual juce::Component* build_core(plugin_controller* controller) = 0;
public:
  virtual void layout() = 0;
  juce::Component* build(plugin_controller* controller);
  juce::Component* component() { return _component.get(); }
  void color(std::int32_t id, juce::Colour color) { _colors[id] = color; }
};

// Just to force a background color.
class container_element:
public ui_element
{
  juce::Colour const _fill;
  std::unique_ptr<ui_element> _content = {};
protected:
  juce::Component* build_core(plugin_controller* controller) override;
public:
  void layout() override;
  container_element(juce::Colour const& fill) : _fill(fill) {}
  void content(std::unique_ptr<ui_element>&& content) { _content = std::move(content); }
};

inline std::unique_ptr<container_element>
create_container_ui(juce::Colour const& fill)
{ return std::make_unique<container_element>(fill); }

class param_element:
public ui_element
{
  base::part_id const _part_id;
  std::int32_t const _param_index;
protected:
  juce::Component* build_core(plugin_controller* controller) override;
public:
  void layout() override {}
  param_element(base::part_id const& part_id, std::int32_t param_index):
  _part_id(part_id), _param_index(param_index) {}
};

inline std::unique_ptr<param_element>
create_param_ui(std::int32_t part_type, std::int32_t part_index, std::int32_t param_index)
{ return std::make_unique<param_element>(part_id(part_type, part_index), param_index); }

class grid_element:
public ui_element
{
  double const _xy_ratio;
  juce::Point<std::int32_t> const _size; // Rows/cols.
  std::vector<std::unique_ptr<ui_element>> _cell_contents = {};
  std::vector<juce::Rectangle<std::int32_t>> _cell_bounds = {};
protected:
  juce::Component* build_core(plugin_controller* controller) override;
public:
  void layout() override;
  std::int32_t pixel_height(std::int32_t pixel_width);
  grid_element(juce::Point<std::int32_t> const& size, double xy_ratio): _xy_ratio(xy_ratio), _size(size) {}
  ui_element* add_cell(std::unique_ptr<ui_element>&& content, std::int32_t row, std::int32_t col, std::int32_t row_span = 1, std::int32_t col_span = 1);
};

inline std::unique_ptr<grid_element>
create_grid_ui(std::int32_t rows, std::int32_t cols, double xy_ratio)
{ return std::make_unique<grid_element>(juce::Point(cols, rows), xy_ratio); }

class root_element:
public ui_element
{
  juce::Colour const _fill;
  std::int32_t const _width; // Pixel size.
  std::unique_ptr<grid_element> _content = {};
protected:
  juce::Component* build_core(plugin_controller* controller) override;
public:
  void layout() override;
  void content(std::unique_ptr<grid_element>&& content) { _content = std::move(content); }
  root_element(std::int32_t width, juce::Colour const& fill) : _fill(fill), _width(width) {}
};

inline std::unique_ptr<root_element>
create_root_ui(std::int32_t width, juce::Colour const& fill)
{ return std::make_unique<root_element>(width, fill); }

} // namespace inf::base::ui

#endif // INF_BASE_UI_UI_HPP