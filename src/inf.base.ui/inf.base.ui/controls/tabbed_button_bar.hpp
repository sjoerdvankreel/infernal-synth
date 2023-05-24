#ifndef INF_BASE_UI_CONTROLS_TABBED_BUTTON_BAR_HPP
#define INF_BASE_UI_CONTROLS_TABBED_BUTTON_BAR_HPP

#include <juce_gui_basics/juce_gui_basics.h>

#include <vector>
#include <cstdint>

namespace inf::base::ui {

class inf_tabbed_button_bar:
public juce::TabbedButtonBar
{
public:
  class listener
  {
  public:
    virtual ~listener() = default;
    virtual void tab_changed(inf_tabbed_button_bar* bar) = 0;
  };

private:
  std::vector<listener*> _listeners = {};

public:
  inf_tabbed_button_bar(): 
  juce::TabbedButtonBar(juce::TabbedButtonBar::TabsAtTop) {}
  void popupMenuClickOnTab(int index, juce::String const& name) override;

  void add_listener(listener* listener) 
  { _listeners.push_back(listener); }
  void currentTabChanged(int new_index, juce::String const& new_name) override 
  { for(std::size_t i = 0; i < _listeners.size(); i++) _listeners[i]->tab_changed(this); }
};

} // namespace inf::base::ui
#endif // INF_BASE_UI_CONTROLS_TABBED_BUTTON_BAR_HPP