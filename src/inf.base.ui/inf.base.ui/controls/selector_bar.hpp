#ifndef INF_BASE_UI_CONTROLS_SELECTOR_BAR_HPP
#define INF_BASE_UI_CONTROLS_SELECTOR_BAR_HPP

#include <inf.base/plugin/plugin_controller.hpp>
#include <juce_gui_basics/juce_gui_basics.h>

#include <vector>
#include <cstdint>

namespace inf::base::ui {

class inf_selector_bar:
public juce::TabbedButtonBar
{
public:
  class listener
  {
  public:
    virtual ~listener() = default;
    virtual void tab_changed(inf_selector_bar* bar) = 0;
  };

private:
  std::int32_t const _part_type;
  std::vector<listener*> _listeners = {};
  inf::base::plugin_controller* const _controller;

public:
  inf_selector_bar(inf::base::plugin_controller* controller, std::int32_t part_type):
  juce::TabbedButtonBar(juce::TabbedButtonBar::TabsAtTop), _part_type(part_type), _controller(controller) {}
  void popupMenuClickOnTab(int index, juce::String const& name) override;

  void add_listener(listener* listener) 
  { _listeners.push_back(listener); }
  void currentTabChanged(int new_index, juce::String const& new_name) override 
  { for(std::size_t i = 0; i < _listeners.size(); i++) _listeners[i]->tab_changed(this); }
};

} // namespace inf::base::ui
#endif // INF_BASE_UI_CONTROLS_SELECTOR_BAR_HPP