#ifndef INF_BASE_UI_UI_HPP
#define INF_BASE_UI_UI_HPP

#include <inf.base/plugin/plugin_controller.hpp>
#include <juce_gui_basics/juce_gui_basics.h>

#include <map>
#include <memory>
#include <cstdint>

namespace inf::base::ui {

class group_component:
public juce::Component
{
  std::uint32_t const _flags;
  std::int32_t const _radius;
  std::int32_t const _padding;
  juce::Colour const _fill;
  juce::Colour const _header;
  juce::Colour const _outline;
public:
  enum flags { none = 0x0, has_fill = 0x1, has_outline = 0x2, has_header = 0x4 };
  void paint(juce::Graphics& g) override;
  group_component(
    std::uint32_t flags, std::int32_t padding, std::int32_t radius, 
    juce::Colour const& fill, juce::Colour const& outline, juce::Colour const& header) : 
  _flags(flags), _padding(padding), _radius(radius), _fill(fill), _header(header), _outline(outline) {}
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

class group_element:
public ui_element
{
  std::uint32_t const _flags;
  std::int32_t const _radius;
  std::int32_t const _padding;
  juce::Colour const _fill;
  juce::Colour const _header;
  juce::Colour const _outline;
  std::unique_ptr<ui_element> _content = {};
protected:
  juce::Component* build_core(plugin_controller* controller) override;
public:
  void layout() override;
  group_element(
    std::uint32_t flags, std::int32_t padding, std::int32_t radius,
    juce::Colour const& fill, juce::Colour const& outline, juce::Colour const& header) : 
  _flags(flags), _padding(padding), _radius(radius), _fill(fill), _header(header), _outline(outline) {}
  void content(std::unique_ptr<ui_element>&& content) { _content = std::move(content); }
};

inline std::unique_ptr<group_element>
create_group_fill_ui(std::int32_t padding, std::int32_t radius, juce::Colour const& fill)
{ return std::make_unique<group_element>(group_component::flags::has_fill, padding, radius, fill, juce::Colour(), juce::Colour()); }
inline std::unique_ptr<group_element>
create_group_outline_ui(std::int32_t padding, std::int32_t radius, juce::Colour const& outline)
{ return std::make_unique<group_element>(group_component::flags::has_outline, padding, radius, juce::Colour(), outline, juce::Colour()); }
inline std::unique_ptr<group_element>
create_group_fill_outline_ui(std::int32_t padding, std::int32_t radius, juce::Colour const& fill, juce::Colour const& outline)
{ return std::make_unique<group_element>(group_component::flags::has_fill | group_component::flags::has_outline, padding, radius, fill, outline, juce::Colour()); }
inline std::unique_ptr<group_element>
create_group_outline_header_ui(std::int32_t padding, std::int32_t radius, juce::Colour const& outline, juce::Colour const& header)
{ return std::make_unique<group_element>(group_component::flags::has_header | group_component::flags::has_outline, padding, radius, juce::Colour(), outline, header); }
inline std::unique_ptr<group_element>
create_group_fill_outline_header_ui(std::int32_t padding, std::int32_t radius, juce::Colour const& fill, juce::Colour const& outline, juce::Colour const& header)
{ return std::make_unique<group_element>(group_component::flags::has_header | group_component::flags::has_outline | group_component::flags::has_fill, padding, radius, fill, outline, header); }

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