#ifndef INF_BASE_UI_CONTROLS_FILE_CHOOSER_DIALOG_HPP
#define INF_BASE_UI_CONTROLS_FILE_CHOOSER_DIALOG_HPP

#include <juce_gui_basics/juce_gui_basics.h>

namespace inf::base::ui {

class inf_file_chooser_dialog: public juce::ResizableWindow,
                                       private juce::FileBrowserListener
{
public:
  inf_file_chooser_dialog(const juce::String& title,
                          const juce::String& instructions,
    juce::FileBrowserComponent& browserComponent,
                          bool warnAboutOverwritingExistingFiles,
    juce::Colour backgroundColour,
                          Component* parentComponent = nullptr);

    ~inf_file_chooser_dialog() override;
    void centreWithDefaultSize (Component* componentToCentreAround = nullptr);
    enum ColourIds { titleTextColourId      = 0x1000850, /**< The colour to use to draw the box's title. */
    };

private:
    class ContentComponent;
    ContentComponent* content;
    const bool warnAboutOverwritingExistingFiles;

    void closeButtonPressed();
    void selectionChanged() override;
    void fileClicked (const juce::File&, const juce::MouseEvent&) override;
    void fileDoubleClicked (const juce::File&) override;
    void browserRootChanged (const juce::File&) override;
    int getDefaultWidth() const;

    void okButtonPressed();
};

} // namespace inf::base::ui
#endif // INF_BASE_UI_CONTROLS_FILE_CHOOSER_DIALOG_HPP