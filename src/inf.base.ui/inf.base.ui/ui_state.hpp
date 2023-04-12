#ifndef INF_BASE_UI_UI_STATE_HPP
#define INF_BASE_UI_UI_STATE_HPP

#include <inf.base/topology/topology_info.hpp>
#include <inf.base.ui/slider_listener.hpp>
#include <juce_gui_basics/juce_gui_basics.h>

#include <vector>
#include <memory>
#include <cstdint>

namespace inf::base::ui {

class ui_state
{
  plugin_controller* const _controller;
  std::vector<std::unique_ptr<juce::Component>> _components;
  std::vector<std::unique_ptr<juce::Slider::Listener>> _listeners;
public:
  void clear();
  template <class component_type> component_type* create_component();
  ui_state(plugin_controller* controller) : _controller(controller) {}
  juce::Slider* create_knob(std::int32_t part, std::int32_t index, std::int32_t param);
};

template <class component_type>
component_type* ui_state::create_component()
{
  std::unique_ptr<component_type> component(std::make_unique<component_type>());
  component_type* result = component.get();
  _components.push_back(std::move(component));
  result->setVisible(true);
  return result;
}

} // namespace inf::base::ui
#endif // INF_BASE_UI_UI_STATE_HPP