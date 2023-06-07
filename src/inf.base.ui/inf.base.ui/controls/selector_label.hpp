#ifndef INF_BASE_UI_CONTROLS_SELECTOR_LABEL_HPP
#define INF_BASE_UI_CONTROLS_SELECTOR_LABEL_HPP

#include <inf.base.ui/controls/label.hpp>
#include <juce_gui_basics/juce_gui_basics.h>

namespace inf::base::ui {

class inf_selector_label:
public inf_label
{
  bool const _vertical;
  std::int32_t const _part_type;
  std::int32_t const _part_count;
  inf::base::plugin_controller* const _controller;
public:
  void paint(juce::Graphics& g) override;
  void mouseUp(juce::MouseEvent const& event) override;

  inf_selector_label(inf::base::plugin_controller* const controller, std::int32_t part_type, std::int32_t part_count, bool vertical):
  inf_label(!vertical), _vertical(vertical), _part_type(part_type), _part_count(part_count), _controller(controller) {}
};

} // namespace inf::base::ui
#endif // INF_BASE_UI_CONTROLS_SELECTOR_LABEL_HPP