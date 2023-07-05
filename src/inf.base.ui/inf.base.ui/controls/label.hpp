#ifndef INF_BASE_UI_CONTROLS_LABEL_HPP
#define INF_BASE_UI_CONTROLS_LABEL_HPP

#include <juce_gui_basics/juce_gui_basics.h>

namespace inf::base::ui {

class inf_label:
public juce::Label
{
  bool _perform_bounds_check;
  bool _bounds_warning_fired = false;
  void check_bounds_warning();
public:
  void resized() override;
  void textWasChanged() override;
  inf_label(bool perform_bounds_check): 
  juce::Label(), _perform_bounds_check(perform_bounds_check) {}
};

} // namespace inf::base::ui
#endif // INF_BASE_UI_CONTROLS_LABEL_HPP