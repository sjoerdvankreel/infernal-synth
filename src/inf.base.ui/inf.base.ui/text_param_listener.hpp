#ifndef INF_BASE_UI_TEXT_PARAM_LISTENER_HPP
#define INF_BASE_UI_TEXT_PARAM_LISTENER_HPP

#include <juce_gui_basics/juce_gui_basics.h>
#include <inf.base/plugin/plugin_controller.hpp>

namespace inf::base::ui 
{

class text_param_listener :
public param_listener
{
  juce::Label* const _label;
  std::int32_t const _param_index;
  plugin_controller* const _controller;

public:
  void param_changed(inf::base::param_value value) override;
  ~text_param_listener() { _controller->remove_param_listener(_param_index, this); }
  text_param_listener(plugin_controller* controller, juce::Label* label, std::int32_t param_index):
  _label(label), _controller(controller), _param_index(param_index) { _controller->add_param_listener(param_index, this); }
};

} // namespace inf::base::ui
#endif // INF_BASE_UI_TEXT_PARAM_LISTENER_HPP