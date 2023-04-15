#ifndef INF_BASE_UI_UI_DESCRIPTION_HPP
#define INF_BASE_UI_UI_DESCRIPTION_HPP

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

class ui_state
{
  std::unique_ptr<root_component> _root;
  std::vector<std::unique_ptr<juce::Component>> _components = {};
public:
  ui_state(): _root(std::make_unique<root_component>()) {}
  template <class Component> Component* create_component();
};

template <class Component>
inline Component* 
ui_state::create_component()
{
  std::unique_ptr<Component> component(std::make_unique<Component>());
  Component* result = component.get();
  _components.push_back(std::move(component));
  return result;
}

class ui_element
{
  // Location in parent grid: row/col/span.
  juce::Rectangle<std::int32_t> _location;
protected:
  ui_element(juce::Rectangle<std::int32_t> const& location): _location(location) {}
public:
  virtual juce::Component* render(ui_state* state) const = 0;
  juce::Rectangle<std::int32_t> const& location() const { return _location; };
};

class grid_element:
public ui_element
{
  // Size is row/cols.
  float const _xy_ratio;
  juce::Point<std::int32_t> _size;
  std::vector<std::unique_ptr<ui_element>> _cells = {};
public:
  float xy_ratio() const { return _xy_ratio; }
  juce::Point<std::int32_t> const& size() const { return _size; };
  grid_element(juce::Point<std::int32_t> const& size, float xy_ratio): _xy_ratio(xy_ratio), _size(size) {}
};

}

#endif // INF_BASE_UI_UI_DESCRIPTION_HPP