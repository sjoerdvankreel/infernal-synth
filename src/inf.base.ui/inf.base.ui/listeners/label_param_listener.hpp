#ifndef INF_BASE_UI_LISTENERS_LABEL_PARAM_LISTENER_HPP
#define INF_BASE_UI_LISTENERS_LABEL_PARAM_LISTENER_HPP

#include <inf.base.ui/shared/support.hpp>
#include <inf.base/plugin/plugin_controller.hpp>

#include <juce_gui_basics/juce_gui_basics.h>

namespace inf::base::ui
{

class label_param_listener :
public param_listener
{
  label_type const _type;
  juce::Label* const _label;
  std::int32_t const _param_index;
  plugin_controller* const _controller;

public:
  void controller_param_changed(inf::base::param_value ui_value) override;
  ~label_param_listener() { _controller->remove_param_listener(_param_index, this); }
  label_param_listener(plugin_controller* controller, juce::Label* label, std::int32_t param_index, label_type type):
  _label(label), _type(type), _controller(controller), _param_index(param_index) { _controller->add_param_listener(param_index, this); }
};

} // namespace inf::base::ui
#endif // INF_BASE_UI_LISTENERS_LABEL_PARAM_LISTENER_HPP