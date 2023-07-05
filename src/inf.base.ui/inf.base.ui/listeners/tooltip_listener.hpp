#ifndef INF_BASE_UI_LISTENERS_TOOLTIP_LISTENER_HPP
#define INF_BASE_UI_LISTENERS_TOOLTIP_LISTENER_HPP

#include <inf.base.ui/shared/support.hpp>
#include <inf.base/plugin/plugin_controller.hpp>

#include <juce_gui_basics/juce_gui_basics.h>

namespace inf::base::ui
{

class tooltip_listener :
public param_listener
{
  std::int32_t const _param_index;
  plugin_controller* const _controller;
  juce::SettableTooltipClient* const _client;

public:
  void controller_param_changed(inf::base::param_value ui_value) override;
  ~tooltip_listener() { _controller->remove_param_listener(_param_index, this); }
  tooltip_listener(plugin_controller* controller, juce::SettableTooltipClient* client, std::int32_t param_index):
  _param_index(param_index), _controller(controller), _client(client) { _controller->add_param_listener(param_index, this); }
};

} // namespace inf::base::ui
#endif // INF_BASE_UI_LISTENERS_TOOLTIP_LISTENER_HPP