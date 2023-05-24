#ifndef INF_BASE_UI_SHARED_UI_HPP
#define INF_BASE_UI_SHARED_UI_HPP

#include <inf.base.ui/shared/support.hpp>
#include <inf.base.ui/shared/look_and_feel.hpp>
#include <inf.base.ui/controls/icon.hpp>
#include <inf.base.ui/controls/slider.hpp>
#include <inf.base.ui/controls/container.hpp>
#include <inf.base.ui/controls/graph_plot.hpp>
#include <inf.base.ui/controls/tabbed_button_bar.hpp>
#include <inf.base.ui/listeners/graph_listener.hpp>
#include <inf.base.ui/listeners/tooltip_listener.hpp>
#include <inf.base.ui/listeners/relevance_listener.hpp>
#include <inf.base.ui/listeners/tab_param_listener.hpp>
#include <inf.base.ui/listeners/tab_button_listener.hpp>
#include <inf.base.ui/listeners/icon_param_listener.hpp>
#include <inf.base.ui/listeners/label_param_listener.hpp>
#include <inf.base.ui/listeners/toggle_param_listener.hpp>
#include <inf.base.ui/listeners/slider_param_listener.hpp>
#include <inf.base.ui/listeners/dropdown_param_listener.hpp>
#include <inf.base/plugin/plugin_controller.hpp>
#include <juce_gui_basics/juce_gui_basics.h>

#include <map>
#include <memory>
#include <cstdint>
#include <functional>

namespace inf::base::ui {

class ui_element
{
  part_id _relevant_if_part = {};
  bool _initially_visible = true;
  bool _hide_if_irrelevant = false;
  std::int32_t _relevant_if_param = -1;
  relevance_selector _relevant_if_selector = nullptr;
  inf::base::plugin_controller* const _controller;
  std::unique_ptr<juce::Component> _component = {};
  std::unique_ptr<relevance_listener> _relevance_listener = {};
protected:
  virtual juce::Component* build_core(juce::LookAndFeel const& lnf) = 0;
  ui_element(inf::base::plugin_controller* controller) : _controller(controller) {}
public:
  virtual void layout() = 0;
  juce::Component* build(juce::LookAndFeel const& lnf);
  juce::Component* component() { return _component.get(); }
  void initially_visible(bool visible) { _initially_visible = visible; }
  inf::base::plugin_controller* controller() const { return _controller; }
  void relevant_if(part_id id, std::int32_t param_index, bool hide_if_irrelevant, relevance_selector selector);
};

class container_element:
public ui_element
{
  std::uint32_t const _flags;
  juce::Point<std::int32_t> _padding;
  std::int32_t const _fill_low_color_id;
  std::int32_t const _fill_high_color_id;
  std::int32_t const _outline_low_color_id;
  std::int32_t const _outline_high_color_id;
  std::unique_ptr<ui_element> _content = {};
protected:
  juce::Component* build_core(juce::LookAndFeel const& lnf) override;
public:
  void layout() override;
  container_element(
    inf::base::plugin_controller* controller, std::unique_ptr<ui_element>&& content, 
    std::uint32_t flags, juce::Point<std::int32_t> const& padding,
    std::int32_t fill_low_color_id, std::int32_t fill_high_color_id,
    std::int32_t outline_low_color_id, std::int32_t outline_high_color_id):
  ui_element(controller), _flags(flags), _padding(padding),
  _fill_low_color_id(fill_low_color_id), _fill_high_color_id(fill_high_color_id),
  _outline_low_color_id(outline_low_color_id), _outline_high_color_id(outline_high_color_id),
  _content(std::move(content)) {}
};

inline std::unique_ptr<container_element>
create_part_group_container_ui(
  inf::base::plugin_controller* controller, std::unique_ptr<ui_element>&& content, juce::Point<std::int32_t> const& padding = container_part_padding)
{ return std::make_unique<container_element>(
  controller, std::move(content), container_component::flags::both, padding,
  inf_look_and_feel::colors::part_group_container_fill_low, inf_look_and_feel::colors::part_group_container_fill_high,
  inf_look_and_feel::colors::part_group_container_outline_low, inf_look_and_feel::colors::part_group_container_outline_high); }

class group_label_element:
public ui_element
{
  bool const _vertical;
  std::string const _text;
protected:
  juce::Component* build_core(juce::LookAndFeel const& lnf) override;
public:
  void layout() override;
  bool vertical() const { return _vertical; }
  group_label_element(inf::base::plugin_controller* controller, std::string const& text, bool vertical):
  ui_element(controller), _vertical(vertical), _text(text) {}
};

inline std::unique_ptr<group_label_element>
create_group_label_ui(inf::base::plugin_controller* controller, std::string const& text, bool vertical)
{ return std::make_unique<group_label_element>(controller, text, vertical); }

class selector_label_element:
public ui_element
{
  std::string const _text;
protected:
  juce::Component* build_core(juce::LookAndFeel const& lnf) override;
public:
  void layout() override {}
  selector_label_element(inf::base::plugin_controller* controller, std::string const& text):
  ui_element(controller), _text(text) {}
};

inline std::unique_ptr<selector_label_element>
create_selector_label_ui(inf::base::plugin_controller* controller, std::string const& text)
{ return std::make_unique<selector_label_element>(controller, text); }

class tab_bar_element :
public ui_element
{
private:
  base::part_id const _part_id;
  std::int32_t const _param_index;
  std::vector<std::string> _headers = {};
  std::unique_ptr<tab_param_listener> _listener = {};
  std::unique_ptr<tab_button_listener> _extra_listener = {};
protected:
  juce::Component* build_core(juce::LookAndFeel const& lnf) override;
public:
  void layout() override {}
  void add_header(std::string const& header) { _headers.push_back(header); }
  void set_extra_listener(std::unique_ptr<tab_button_listener>&& listener) { _extra_listener = std::move(listener); }
  tab_bar_element(inf::base::plugin_controller* controller, base::part_id const& part_id, 
    std::int32_t param_index): ui_element(controller), _part_id(part_id), _param_index(param_index) {}
};

inline std::unique_ptr<tab_bar_element>
create_tab_bar(inf::base::plugin_controller* controller, base::part_id const& part_id, std::int32_t param_index)
{ return std::make_unique<tab_bar_element>(controller, part_id, param_index); }

class param_label_element:
public ui_element
{
private:
  label_type const _type;
  base::part_id const _part_id;
  std::int32_t const _param_index;
  juce::Justification _justification;
  std::unique_ptr<label_param_listener> _listener = {};
protected:
  juce::Component* build_core(juce::LookAndFeel const& lnf) override;
public:
  void layout() override {}
  param_label_element(inf::base::plugin_controller* controller, base::part_id const& part_id, 
    std::int32_t param_index, label_type type, juce::Justification justification):
  ui_element(controller), _type(type), _part_id(part_id), 
  _param_index(param_index), _justification(justification) {}
};

inline std::unique_ptr<param_label_element>
create_param_label_ui(inf::base::plugin_controller* controller, std::int32_t part_type, 
  std::int32_t part_index, std::int32_t param_index, label_type type, juce::Justification justification)
{ return std::make_unique<param_label_element>(controller, part_id(part_type, part_index), param_index, type, justification); }

class param_icon_element:
public ui_element
{
  std::int32_t const _value;
  icon_selector const _selector;
  base::part_id const _part_id;
  std::int32_t const _param_index;
  std::unique_ptr<icon_param_listener> _listener = {};
protected:
  juce::Component* build_core(juce::LookAndFeel const& lnf) override;
public:
  void layout() override {}
  param_icon_element(
    inf::base::plugin_controller* controller, base::part_id const& part_id, 
    std::int32_t param_index, std::int32_t value, icon_selector selector):
  ui_element(controller), _value(value), _selector(selector), _part_id(part_id), _param_index(param_index) {}
};

std::unique_ptr<param_icon_element>
create_param_icon_ui(
  inf::base::plugin_controller* controller, std::int32_t part_type, 
  std::int32_t part_index, std::int32_t param_index, icon_selector selector);
inline std::unique_ptr<param_icon_element>
create_param_icon_ui(inf::base::plugin_controller* controller, icon_type type)
{ return std::make_unique<param_icon_element>(controller, part_id(-1, -1), -1, static_cast<std::int32_t>(type), nullptr); }

class param_edit_element :
public ui_element
{
  edit_type const _type;
  bool const _force_toggle_on;
  base::part_id const _part_id;
  std::int32_t const _param_index;
  tooltip_type const _tooltip_type;
  std::unique_ptr<tooltip_listener> _tooltip_listener = {};
  std::unique_ptr<toggle_param_listener> _toggle_listener = {};
  std::unique_ptr<slider_param_listener> _slider_listener = {};
  std::unique_ptr<dropdown_param_listener> _dropdown_listener = {};
  juce::Component* build_toggle_core(juce::LookAndFeel const& lnf);
  juce::Component* build_slider_core(juce::LookAndFeel const& lnf);
  juce::Component* build_dropdown_core(juce::LookAndFeel const& lnf);
protected:
  juce::Component* build_core(juce::LookAndFeel const& lnf) override;
public:
  void layout() override;
  param_edit_element(
    inf::base::plugin_controller* controller, 
    base::part_id const& part_id, std::int32_t param_index, edit_type type, tooltip_type tooltip_type, bool force_toggle_on):
  ui_element(controller), _type(type), _force_toggle_on(force_toggle_on), _part_id(part_id), _param_index(param_index), _tooltip_type(tooltip_type) {}
};

inline std::unique_ptr<param_edit_element>
create_param_edit_ui(
  inf::base::plugin_controller* controller, std::int32_t part_type,
  std::int32_t part_index, std::int32_t param_index, edit_type type, tooltip_type tooltip_type, bool force_toggle_on = false)
{ return std::make_unique<param_edit_element>(controller, part_id(part_type, part_index), param_index, type, tooltip_type, force_toggle_on); }

class part_graph_element:
public ui_element
{
  base::part_id const _part_id;
  std::int32_t const _graph_type;
  std::int32_t const _tooltip_param;
  std::unique_ptr<graph_listener> _listener = {};
protected:
  juce::Component* build_core(juce::LookAndFeel const& lnf) override;
public:
  void layout() override {}
  part_graph_element(inf::base::plugin_controller* controller, part_id part_id, std::int32_t graph_type, std::int32_t tooltip_param):
  ui_element(controller), _part_id(part_id), _graph_type(graph_type), _tooltip_param(tooltip_param) {}
};

inline std::unique_ptr<part_graph_element>
create_part_graph_ui(
  inf::base::plugin_controller* controller, std::int32_t part_type,
  std::int32_t part_index, std::int32_t graph_type, std::int32_t tooltip_param)
{ return std::make_unique<part_graph_element>(controller, part_id(part_type, part_index), graph_type, tooltip_param); }

class grid_element:
public ui_element
{
  std::vector<juce::Grid::TrackInfo> const _row_distribution;
  std::vector<juce::Grid::TrackInfo> const _column_distribution;
  std::vector<std::unique_ptr<ui_element>> _cell_contents = {};
  std::vector<juce::Rectangle<std::int32_t>> _cell_bounds = {};

protected:
  juce::Component* build_core(juce::LookAndFeel const& lnf) override;

public:
  void layout() override;

  ui_element* add_cell(
    std::unique_ptr<ui_element>&& content, std::int32_t row,
    std::int32_t col, std::int32_t row_span = 1, std::int32_t col_span = 1);

  grid_element(
    inf::base::plugin_controller* controller,
    std::vector<juce::Grid::TrackInfo> const& row_distribution, 
    std::vector<juce::Grid::TrackInfo> const& column_distribution):
  ui_element(controller), _row_distribution(row_distribution), _column_distribution(column_distribution) {}
};

std::unique_ptr<grid_element>
create_grid_ui(
  inf::base::plugin_controller* controller,
  std::vector<std::int32_t> const& row_distribution_relative, 
  std::vector<std::int32_t> const& column_distribution_relative);

inline std::unique_ptr<grid_element>
create_grid_ui(inf::base::plugin_controller* controller, std::int32_t rows, std::int32_t columns)
{ return create_grid_ui(controller,
  std::vector<std::int32_t>(rows, 1), 
  std::vector<std::int32_t>(columns, 1)); }

inline std::unique_ptr<grid_element>
create_grid_ui(inf::base::plugin_controller* controller,
  std::vector<juce::Grid::TrackInfo> const& row_distribution, 
  std::vector<juce::Grid::TrackInfo> const& column_distribution)
{ return std::make_unique<grid_element>(controller, row_distribution, column_distribution); }

class root_element:
public ui_element
{
  inf_look_and_feel _lnf;
  juce::Colour const _fill;
  std::int32_t const _width; // Pixel size.
  std::unique_ptr<ui_element> _content = {};
  std::unique_ptr<juce::TooltipWindow> _tooltip = {};
protected:
  juce::Component* build_core(juce::LookAndFeel const& lnf) override;
public:
  void layout() override;
  inf_look_and_feel& look_and_feel() { return _lnf; }
  juce::Component* build() { return ui_element::build(_lnf); }

  ~root_element() { component()->setLookAndFeel(nullptr); }
  root_element(inf::base::plugin_controller* controller, std::unique_ptr<ui_element>&& content, std::int32_t width, juce::Colour const& fill) :
  ui_element(controller), _lnf(controller), _fill(fill), _width(width), _content(std::move(content)) {}
};

inline std::unique_ptr<root_element>
create_root_ui(
  inf::base::plugin_controller* controller, 
  std::unique_ptr<ui_element>&& content, std::int32_t width, juce::Colour const& fill)
{ return std::make_unique<root_element>(controller, std::move(content), width, fill); }

std::unique_ptr<ui_element>
create_part_single_ui(
  plugin_controller* controller, std::string const& header, std::unique_ptr<ui_element>&& part);
std::unique_ptr<ui_element>
create_part_selector_ui(
  plugin_controller* controller, std::string const& header, std::int32_t selector_part_type, std::int32_t selector_param_index,
  std::int32_t label_columns, std::int32_t selector_columns, std::vector<std::unique_ptr<ui_element>>&& selected_parts);

std::unique_ptr<ui_element>
create_part_group_ui(
  plugin_controller* controller, std::unique_ptr<group_label_element>&& label,
  std::unique_ptr<ui_element>&& content);
std::unique_ptr<ui_element>
create_labeled_param_ui(
  plugin_controller* controller, std::int32_t part_type, std::int32_t part_index, std::int32_t param_index, 
  edit_type edit_type, label_type label_type, tooltip_type tooltip_type, bool force_toggle_on = false, std::int32_t hslider_cols = -1);
std::unique_ptr<ui_element>
create_iconed_param_ui(
  plugin_controller* controller, std::int32_t part_type, std::int32_t part_index, std::int32_t param_index, 
  edit_type edit_type, icon_type icon_type, tooltip_type tooltip_type, bool force_toggle_on = false);
std::unique_ptr<ui_element>
create_iconed_param_ui(
  plugin_controller* controller, std::int32_t part_type, std::int32_t part_index, std::int32_t param_index, 
  edit_type edit_type, icon_selector icon_selector, tooltip_type tooltip_type, bool force_toggle_on = false);
std::unique_ptr<ui_element>
create_param_ui(
  plugin_controller* controller, std::unique_ptr<ui_element>&& label_or_icon, std::int32_t part_type, 
  std::int32_t part_index, std::int32_t param_index, edit_type edit_type, tooltip_type tooltip_type, bool force_toggle_on = false, std::int32_t hslider_cols = -1);

} // namespace inf::base::ui
#endif // INF_BASE_UI_SHARED_UI_HPP