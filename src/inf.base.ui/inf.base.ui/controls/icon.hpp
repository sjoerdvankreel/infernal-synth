#ifndef INF_BASE_UI_CONTROLS_ICON_HPP
#define INF_BASE_UI_CONTROLS_ICON_HPP

#include <inf.base.ui/shared/support.hpp>
#include <juce_gui_basics/juce_gui_basics.h>

namespace inf::base::ui {

class inf_icon:
public juce::Component
{
  icon_type _type;
public:
  void paint(juce::Graphics& g) override;
  inf_icon(icon_type type): _type(type) {}
};

} // namespace inf::base::ui
#endif // INF_BASE_UI_CONTROLS_ICON_HPP