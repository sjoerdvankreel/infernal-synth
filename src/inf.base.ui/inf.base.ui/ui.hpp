#ifndef INF_BASE_UI_UI_HPP
#define INF_BASE_UI_UI_HPP

#include <inf.base.ui/look_and_feel.hpp>
#include <inf.base.ui/slider_listener.hpp>
#include <inf.base/plugin/plugin_controller.hpp>
#include <juce_gui_basics/juce_gui_basics.h>

#include <map>
#include <memory>
#include <cstdint>

namespace inf::base::ui {

class container_component:
public juce::Component
{
  float const _radius;
  float const _thickness;
  juce::Colour const _fill;
  juce::Colour const _outline;
  std::uint32_t const _flags;
public:
  enum flags { none = 0x0, fill = 0x1, outline = 0x2 };
  void paint(juce::Graphics& g) override;
  container_component(std::uint32_t flags, float radius, float thickness, juce::Colour const& fill, juce::Colour const& outline): 
  _radius(radius), _thickness(thickness), _fill(fill), _outline(outline), _flags(flags) {}
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

class container_element:
public ui_element
{
  float const _radius;
  float const _thickness;
  juce::Colour const _fill;
  juce::Colour const _outline;
  std::uint32_t const _flags;
  std::unique_ptr<ui_element> _content = {};
protected:
  juce::Component* build_core(plugin_controller* controller) override;
public:
  void layout() override;
  void content(std::unique_ptr<ui_element>&& content) { _content = std::move(content); }
  container_element(std::uint32_t flags, float radius, float thickness, juce::Colour const& fill, juce::Colour const& outline) :
  _radius(radius), _thickness(thickness), _fill(fill), _outline(outline), _flags(flags) {}
};

inline std::unique_ptr<container_element>
create_container_fill_ui(float radius, juce::Colour const& fill)
{ return std::make_unique<container_element>(container_component::flags::fill, radius, 0.0f, fill, juce::Colour()); }
inline std::unique_ptr<container_element>
create_container_outline_ui(float radius, float thickness, juce::Colour const& outline)
{ return std::make_unique<container_element>(container_component::flags::outline, radius, thickness, juce::Colour(), outline); }
inline std::unique_ptr<container_element>
create_container_fill_outline_ui(float radius, float thickness, juce::Colour const& fill, juce::Colour const& outline)
{ return std::make_unique<container_element>(container_component::flags::outline | container_component::flags::fill, radius, thickness, fill, outline); }

class param_slider_element:
public ui_element
{
  base::part_id const _part_id;
  std::int32_t const _param_index;
  std::unique_ptr<slider_listener> _listener = {};
protected:
  juce::Component* build_core(plugin_controller* controller) override;
public:
  void layout() override {}
  param_slider_element(base::part_id const& part_id, std::int32_t param_index):
  _part_id(part_id), _param_index(param_index) {}
};

inline std::unique_ptr<param_slider_element>
create_param_slider_ui(std::int32_t part_type, std::int32_t part_index, std::int32_t param_index)
{ return std::make_unique<param_slider_element>(part_id(part_type, part_index), param_index); }

class param_label_element:
public ui_element
{
  base::part_id const _part_id;
  std::int32_t const _param_index;
protected:
  juce::Component* build_core(plugin_controller* controller) override;
public:
  void layout() override {}
  param_label_element(base::part_id const& part_id, std::int32_t param_index):
  _part_id(part_id), _param_index(param_index) {}
};

inline std::unique_ptr<param_label_element>
create_param_label_ui(std::int32_t part_type, std::int32_t part_index, std::int32_t param_index)
{ return std::make_unique<param_label_element>(part_id(part_type, part_index), param_index); }

class grid_element:
public ui_element
{
  float const _gap_size;
  float const _xy_ratio;
  juce::Point<std::int32_t> const _size; // Rows/cols.
  std::vector<std::unique_ptr<ui_element>> _cell_contents = {};
  std::vector<juce::Rectangle<std::int32_t>> _cell_bounds = {};
protected:
  juce::Component* build_core(plugin_controller* controller) override;
public:
  void layout() override;
  std::int32_t pixel_height(std::int32_t pixel_width);
  grid_element(juce::Point<std::int32_t> const& size, float gap_size, float xy_ratio): _gap_size(gap_size), _xy_ratio(xy_ratio), _size(size) {}
  ui_element* add_cell(std::unique_ptr<ui_element>&& content, std::int32_t row, std::int32_t col, std::int32_t row_span = 1, std::int32_t col_span = 1);
};

inline std::unique_ptr<grid_element>
create_grid_ui(std::int32_t rows, std::int32_t cols, float gap_size, float xy_ratio)
{ return std::make_unique<grid_element>(juce::Point(cols, rows), gap_size, xy_ratio); }

class root_element:
public ui_element
{
  inf_look_and_feel _lnf = {};
  juce::Colour const _fill;
  std::int32_t const _width; // Pixel size.
  std::unique_ptr<grid_element> _content = {};
protected:
  juce::Component* build_core(plugin_controller* controller) override;
public:
  void layout() override;
  inf_look_and_feel& look_and_feel() { return _lnf; }
  void content(std::unique_ptr<grid_element>&& content) { _content = std::move(content); }
  root_element(std::int32_t width, juce::Colour const& fill) : _fill(fill), _width(width) {}
};

inline std::unique_ptr<root_element>
create_root_ui(std::int32_t width, juce::Colour const& fill)
{ return std::make_unique<root_element>(width, fill); }

} // namespace inf::base::ui

#endif // INF_BASE_UI_UI_HPP