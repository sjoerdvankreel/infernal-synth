#ifndef INF_BASE_UI_CONTROLS_PARAM_DROPDOWN_HPP
#define INF_BASE_UI_CONTROLS_PARAM_DROPDOWN_HPP

#include <inf.base.ui/shared/support.hpp>
#include <inf.base/topology/param_descriptor.hpp>
#include <juce_gui_basics/juce_gui_basics.h>
#include <cstdint>

namespace inf::base::ui {

inline std::int32_t constexpr dropdown_id_offset = 1000;
  
class inf_param_dropdown:
public juce::ComboBox
{
private:
  std::int32_t const _param_index;
  base::plugin_controller* const _controller;
public:
  void mouseUp(juce::MouseEvent const& e) override;
  std::int32_t param_index() const { return _param_index; }

  inf_param_dropdown(base::plugin_controller* controller, std::int32_t param_index):
  _param_index(param_index), _controller(controller) {}
};

} // namespace inf::base::ui
#endif // INF_BASE_UI_CONTROLS_PARAM_DROPDOWN_HPP