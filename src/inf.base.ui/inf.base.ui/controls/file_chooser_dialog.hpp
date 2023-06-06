#ifndef INF_BASE_UI_CONTROLS_FILE_CHOOSER_DIALOG_HPP
#define INF_BASE_UI_CONTROLS_FILE_CHOOSER_DIALOG_HPP

#include <juce_gui_basics/juce_gui_basics.h>
#include <inf.base/plugin/plugin_controller.hpp>

namespace inf::base::ui {

class inf_file_chooser_dialog: public juce::ResizableWindow,
                                       private juce::FileBrowserListener
{
public:
  inf_file_chooser_dialog(inf::base::plugin_controller* controller, const juce::String& title,
    juce::FileBrowserComponent& browserComponent,
    juce::Colour backgroundColour,
                          juce::Component* parentComponent);

    ~inf_file_chooser_dialog() override;
    void centreWithDefaultSize (Component* componentToCentreAround) { centreAroundComponent(componentToCentreAround, 600, 500); }
    enum ColourIds { titleTextColourId      = 0x1000850, /**< The colour to use to draw the box's title. */
    };

private:
    inf::base::plugin_controller* _controller;
    class ContentComponent;
    ContentComponent* content;
    void selectionChanged() override;
    void fileClicked (const juce::File&, const juce::MouseEvent&) override {}
    void fileDoubleClicked (const juce::File&) override;
    void browserRootChanged (const juce::File&) override {}
};

} // namespace inf::base::ui
#endif // INF_BASE_UI_CONTROLS_FILE_CHOOSER_DIALOG_HPP