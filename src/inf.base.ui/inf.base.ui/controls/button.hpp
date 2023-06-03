#ifndef INF_BASE_UI_CONTROLS_BUTTON_HPP
#define INF_BASE_UI_CONTROLS_BUTTON_HPP

#include <inf.base/plugin/plugin_controller.hpp>
#include <juce_gui_basics/juce_gui_basics.h>

namespace inf::base::ui {

class inf_button:
public juce::TextButton
{
  plugin_controller* const _controller;
public:
  void paint(juce::Graphics& g) override;
  inf_button(plugin_controller* controller): _controller(controller) {}
};

} // namespace inf::base::ui
#endif // INF_BASE_UI_CONTROLS_BUTTON_HPP