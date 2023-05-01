#ifndef INF_BASE_UI_SHARED_UI_HPP
#define INF_BASE_UI_SHARED_UI_HPP

#include <inf.base.ui/shared/support.hpp>
#include <inf.base.ui/shared/look_and_feel.hpp>
#include <inf.base.ui/controls/slider.hpp>
#include <inf.base.ui/controls/container.hpp>
#include <inf.base.ui/listeners/label_param_listener.hpp>
#include <inf.base.ui/listeners/toggle_param_listener.hpp>
#include <inf.base.ui/listeners/slider_param_listener.hpp>
#include <inf.base/plugin/plugin_controller.hpp>
#include <juce_gui_basics/juce_gui_basics.h>

#include <map>
#include <memory>
#include <cstdint>

namespace inf::base::ui {

class ui_element
{
  std::unique_ptr<juce::Component> _component = {};
  std::map<std::int32_t, juce::Colour> _colors = {};
protected:
  virtual juce::Component* 
  build_core(plugin_controller* controller, juce::LookAndFeel const& lnf) = 0;
public:
  virtual void layout() = 0;
  juce::Component* build(plugin_controller* controller, juce::LookAndFeel const& lnf);
  juce::Component* component() { return _component.get(); }
  void color(std::int32_t id, juce::Colour color) { _colors[id] = color; }
};

class container_element:
public ui_element
{
  juce::Colour const _fill;
  juce::Colour const _outline;
  std::uint32_t const _flags;
  std::unique_ptr<ui_element> _content = {};
protected:
  juce::Component* build_core(plugin_controller* controller, juce::LookAndFeel const& lnf) override;
public:
  void layout() override;
  container_element(std::unique_ptr<ui_element>&& content, std::uint32_t flags, juce::Colour const& fill, juce::Colour const& outline) :
  _fill(fill), _outline(outline), _flags(flags), _content(std::move(content)) {}
};

inline std::unique_ptr<container_element>
create_container_fill_ui(
  std::unique_ptr<ui_element>&& content, juce::Colour const& fill)
{ return std::make_unique<container_element>(std::move(content), container_component::flags::fill, fill, juce::Colour()); }
inline std::unique_ptr<container_element>
create_container_outline_ui(
  std::unique_ptr<ui_element>&& content, juce::Colour const& outline)
{ return std::make_unique<container_element>(std::move(content), container_component::flags::outline, juce::Colour(), outline); }
inline std::unique_ptr<container_element>
create_container_fill_outline_ui(
  std::unique_ptr<ui_element>&& content, juce::Colour const& fill, juce::Colour const& outline)
{ return std::make_unique<container_element>(std::move(content), container_component::flags::outline | container_component::flags::fill, fill, outline); }

class group_label_element:
public ui_element
{
  bool const _vertical;
  std::string const _text;
protected:
  juce::Component* build_core(plugin_controller* controller, juce::LookAndFeel const& lnf) override;
public:
  void layout() override;
  bool vertical() const { return _vertical; }
  group_label_element(std::string const& text, bool vertical):
  _vertical(vertical), _text(text) {}
};

inline std::unique_ptr<group_label_element>
create_group_label_ui(std::string const& text, bool vertical)
{ return std::make_unique<group_label_element>(text, vertical); }

class param_label_element:
public ui_element
{
private:
  label_type const _type;
  base::part_id const _part_id;
  std::int32_t const _param_index;
  juce::Justification _justification;
  std::unique_ptr<label_param_listener> _listener = {};
public:
  void layout() override {}
  param_label_element(base::part_id const& part_id, std::int32_t param_index, label_type type, juce::Justification justification):
  _type(type), _part_id(part_id), _param_index(param_index), _justification(justification) {}
protected:
  juce::Component* build_core(plugin_controller* controller, juce::LookAndFeel const& lnf) override;
};

inline std::unique_ptr<param_label_element>
create_param_label_ui(
  std::int32_t part_type, std::int32_t part_index, std::int32_t param_index, 
  label_type type, juce::Justification justification = juce::Justification::centred)
{ return std::make_unique<param_label_element>(part_id(part_type, part_index), param_index, type, justification); }

class param_edit_element :
public ui_element
{
  edit_type const _type;
  base::part_id const _part_id;
  std::int32_t const _param_index;
  std::unique_ptr<toggle_param_listener> _toggle_listener = {};
  std::unique_ptr<slider_param_listener> _slider_listener = {};
  juce::Component* build_toggle_core(plugin_controller* controller, juce::LookAndFeel const& lnf);
  juce::Component* build_slider_core(plugin_controller* controller, juce::LookAndFeel const& lnf);
public:
  void layout() override;
  param_edit_element(base::part_id const& part_id, std::int32_t param_index, edit_type type):
  _type(type), _part_id(part_id), _param_index(param_index) {}
protected:
  juce::Component* build_core(plugin_controller* controller, juce::LookAndFeel const& lnf) override
  { return _type == edit_type::toggle? build_toggle_core(controller, lnf): build_slider_core(controller, lnf); }
};

inline std::unique_ptr<param_edit_element>
create_param_edit_ui(std::int32_t part_type, std::int32_t part_index, std::int32_t param_index, edit_type type)
{ return std::make_unique<param_edit_element>(part_id(part_type, part_index), param_index, type); }

class grid_element:
public ui_element
{
  std::vector<juce::Grid::TrackInfo> const _row_distribution;
  std::vector<juce::Grid::TrackInfo> const _column_distribution;
  std::vector<std::unique_ptr<ui_element>> _cell_contents = {};
  std::vector<juce::Rectangle<std::int32_t>> _cell_bounds = {};
public:
  void layout() override;
  std::int32_t pixel_height(std::int32_t pixel_width);

  grid_element(
    std::vector<juce::Grid::TrackInfo> const& row_distribution, 
    std::vector<juce::Grid::TrackInfo> const& column_distribution):
  _row_distribution(row_distribution), _column_distribution(column_distribution) {}

  ui_element* add_cell(
    std::unique_ptr<ui_element>&& content, std::int32_t row, 
    std::int32_t col, std::int32_t row_span = 1, std::int32_t col_span = 1);
protected:
  juce::Component* build_core(plugin_controller* controller, juce::LookAndFeel const& lnf) override;
};

std::unique_ptr<grid_element>
create_grid_ui(
  std::vector<std::int32_t> const& row_distribution_relative, 
  std::vector<std::int32_t> const& column_distribution_relative);

inline std::unique_ptr<grid_element>
create_grid_ui(std::int32_t rows, std::int32_t columns)
{ return create_grid_ui(
  std::vector<std::int32_t>(rows, 1), 
  std::vector<std::int32_t>(columns, 1)); }

inline std::unique_ptr<grid_element>
create_grid_ui(
  std::vector<juce::Grid::TrackInfo> const& row_distribution, 
  std::vector<juce::Grid::TrackInfo> const& column_distribution)
{ return std::make_unique<grid_element>(row_distribution, column_distribution); }

class root_element:
public ui_element
{
  juce::Colour const _fill;
  std::int32_t const _width; // Pixel size.
  inf_look_and_feel _lnf = {};
  std::unique_ptr<grid_element> _content = {};
protected:
  juce::Component* build_core(plugin_controller* controller, juce::LookAndFeel const& lnf) override;
public:
  ~root_element() { component()->setLookAndFeel(nullptr); }
  root_element(std::unique_ptr<grid_element>&& content, std::int32_t width, juce::Colour const& fill) : 
  _fill(fill), _width(width), _content(std::move(content)) {}

  void layout() override;
  inf_look_and_feel& look_and_feel() { return _lnf; }
  juce::Component* build(plugin_controller* controller) { return ui_element::build(controller, _lnf); }
};

inline std::unique_ptr<root_element>
create_root_ui(std::unique_ptr<grid_element>&& content, std::int32_t width, juce::Colour const& fill)
{ return std::make_unique<root_element>(std::move(content), width, fill); }

std::unique_ptr<ui_element>
create_group_ui(std::unique_ptr<group_label_element>&& label, std::unique_ptr<ui_element>&& content);
std::unique_ptr<ui_element>
create_param_ui(std::int32_t part_type, std::int32_t part_index, std::int32_t param_index, label_type label_type, edit_type edit_type);

} // namespace inf::base::ui

#endif // INF_BASE_UI_SHARED_UI_HPP