#ifndef INF_BASE_UI_CONTROLS_TABBED_BUTTON_BAR_HPP
#define INF_BASE_UI_CONTROLS_TABBED_BUTTON_BAR_HPP

#include <juce_gui_basics/juce_gui_basics.h>
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
  listener* _listener = nullptr;

public:
  inf_tabbed_button_bar(): 
  juce::TabbedButtonBar(juce::TabbedButtonBar::TabsAtTop) {}

  void set_listener(listener* listener) 
  { _listener = listener; }
  void currentTabChanged(int new_index, juce::String const& new_name) override 
  { if(_listener) _listener->tab_changed(this); }
};

} // namespace inf::base::ui
#endif // INF_BASE_UI_CONTROLS_TABBED_BUTTON_BAR_HPP