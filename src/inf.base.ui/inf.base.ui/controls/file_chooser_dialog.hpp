#ifndef INF_BASE_UI_CONTROLS_FILE_CHOOSER_DIALOG_HPP
#define INF_BASE_UI_CONTROLS_FILE_CHOOSER_DIALOG_HPP

#include <juce_gui_basics/juce_gui_basics.h>
#include <inf.base/plugin/plugin_controller.hpp>

namespace inf::base::ui {

class inf_file_chooser_dialog: 
public juce::ResizableWindow,
private juce::FileBrowserListener
{
private:
  class content_component;
  content_component* content;
  inf::base::plugin_controller* _controller;

  void selectionChanged() override;
  void fileDoubleClicked(juce::File const&) override;
  void browserRootChanged(juce::File const&) override {}
  void fileClicked(juce::File const&, juce::MouseEvent const&) override {}

public:
  ~inf_file_chooser_dialog() override;
  inf_file_chooser_dialog(
    inf::base::plugin_controller* controller, juce::String const& title,
    juce::FileBrowserComponent& browser_component,
    juce::Colour background_colour, juce::Component* parent_component);
  void center_around(juce::Component* component) { centreAroundComponent(component, 600, 500); }
};

} // namespace inf::base::ui
#endif // INF_BASE_UI_CONTROLS_FILE_CHOOSER_DIALOG_HPP