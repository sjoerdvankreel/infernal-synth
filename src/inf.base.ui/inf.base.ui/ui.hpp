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
  std::unique_ptr<juce::Component> _component = {};
protected:
  virtual juce::Component* render_core() = 0;
public:
  juce::Component* render();
  juce::Component* get() { return _component.get(); }
};

class cell_element:
public ui_element
{
  juce::Rectangle<std::int32_t> _bounds; // Row/col/span bounds.
public:
  juce::Rectangle<std::int32_t> const& bounds() const { return _bounds; }
  void bounds(juce::Rectangle<std::int32_t> const& bounds) { _bounds = bounds; }
};

class param_element:
public cell_element
{
  std::int32_t const _param_index;
  plugin_controller* const _controller;
protected:
  juce::Component* render_core() override;
public:
  param_element(plugin_controller* controller, std::int32_t param_index) : 
  _param_index(param_index), _controller(controller) {}
};

class grid_element:
public ui_element
{
  float _xy_ratio;
  juce::Point<std::int32_t> _size; // Rows/cols.
  std::vector<std::unique_ptr<ui_element>> _cells = {};
protected:
  juce::Component* render_core() override;
public:
  float xy_ratio(float xy_ratio) { _xy_ratio = xy_ratio; }
  void size(juce::Point<std::int32_t> const& size) { _size = size; }
};

class root_element:
public ui_element
{
  juce::Point<std::int32_t> _size; // Pixel size.
  std::unique_ptr<ui_element> _content;
protected:
  juce::Component* render_core() override;
public:
  void size(juce::Point<std::int32_t> const& size) { _size = size; }
  void content(std::unique_ptr<ui_element>&& content) { _content = std::move(content); }
};

} // namespace inf::base::ui

#endif // INF_BASE_UI_UI_HPP