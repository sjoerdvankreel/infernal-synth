#ifndef INF_BASE_UI_CONTROLS_TOGGLE_BUTTON_HPP
#define INF_BASE_UI_CONTROLS_TOGGLE_BUTTON_HPP

#include <inf.base.ui/shared/support.hpp>
#include <juce_gui_basics/juce_gui_basics.h>

namespace inf::base::ui {

class inf_toggle_button:
public juce::ToggleButton
{
  bool const _force_on;
  std::int32_t const _param_index;
  base::plugin_controller* const _controller;

public:
  bool force_on() const { return _force_on; }
  void mouseUp(juce::MouseEvent const& e) override;

  inf_toggle_button(base::plugin_controller* controller, std::int32_t param_index, bool force_on):
  juce::ToggleButton(), _force_on(force_on), _param_index(param_index), _controller(controller) {}
};

} // namespace inf::base::ui
#endif // INF_BASE_UI_CONTROLS_TOGGLE_BUTTON_HPP