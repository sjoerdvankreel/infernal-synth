#ifndef INF_BASE_UI_CONTROLS_TABBED_COMPONENT_HPP
#define INF_BASE_UI_CONTROLS_TABBED_COMPONENT_HPP

#include <juce_gui_basics/juce_gui_basics.h>

namespace inf::base::ui {

class inf_tabbed_component:
public juce::TabbedComponent
{
  juce::Component* _bar_extra_after = nullptr;
  juce::Component* _bar_extra_before = nullptr;
public: 
  void bar_extra_after(juce::Component* component) { _bar_extra_after = component; }
  void bar_extra_before(juce::Component* component) { _bar_extra_before = component; }
  juce::Component* bar_extra_after(juce::Component* component) const { return _bar_extra_after; }
  juce::Component* bar_extra_before(juce::Component* component) const { return _bar_extra_before; }
  inf_tabbed_component(juce::TabbedButtonBar::Orientation orientation): juce::TabbedComponent(orientation) {}
};

} // namespace inf::base::ui
#endif // INF_BASE_UI_CONTROLS_TABBED_COMPONENT_HPP