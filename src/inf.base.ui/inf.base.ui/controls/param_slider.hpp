#ifndef INF_BASE_UI_CONTROLS_PARAM_SLIDER_HPP
#define INF_BASE_UI_CONTROLS_PARAM_SLIDER_HPP

#include <inf.base/plugin/plugin_controller.hpp>
#include <inf.base.ui/shared/support.hpp>
#include <inf.base/topology/param_descriptor.hpp>
#include <inf.base.ui/shared/look_and_feel.hpp>
#include <juce_gui_basics/juce_gui_basics.h>
#include <cstdint>

namespace inf::base::ui {
  
class inf_param_slider:
public juce::Slider
{
private:
  edit_type _type;
  std::int32_t const _param_index;
  base::plugin_controller* const _controller;
  lnf_factory const _lnf_factory;

public:
  void mouseUp(juce::MouseEvent const& e) override;
  void mouseDown(juce::MouseEvent const& e) override;

  edit_type type() const { return _type; }
  std::int32_t param_index() const { return _param_index; }

  inf_param_slider(base::plugin_controller* controller, std::int32_t param_index, edit_type type, lnf_factory lnf_factory):
  _type(type), _param_index(param_index), _controller(controller), _lnf_factory(lnf_factory) {}
};

} // namespace inf::base::ui
#endif // INF_BASE_UI_CONTROLS_PARAM_SLIDER_HPP