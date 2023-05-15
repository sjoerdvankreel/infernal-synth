#ifndef INF_BASE_UI_CONTROLS_SELECTOR_LABEL_HPP
#define INF_BASE_UI_CONTROLS_SELECTOR_LABEL_HPP

#include <inf.base.ui/controls/label.hpp>
#include <juce_gui_basics/juce_gui_basics.h>

namespace inf::base::ui {

class inf_selector_label:
public inf_label
{
public:
  void paint(juce::Graphics& g) override;
  inf_selector_label(): inf_label(true) {}
};

} // namespace inf::base::ui
#endif // INF_BASE_UI_CONTROLS_SELECTOR_LABEL_HPP