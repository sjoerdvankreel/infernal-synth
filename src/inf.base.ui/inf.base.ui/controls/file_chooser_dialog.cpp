#include <inf.base.ui/controls/file_chooser_dialog.hpp>

using namespace juce;

namespace juce {
bool juce_areThereAnyAlwaysOnTopWindows();
};

namespace inf::base::ui {

class inf_file_chooser_dialog::ContentComponent  : public Component
{
public:
    ContentComponent (const String& name, const String& desc, FileBrowserComponent& chooser)
        : Component (name),
          chooserComponent (chooser),
          okButton (chooser.getActionVerb()),
          cancelButton (TRANS ("Cancel")),
          instructions (desc)
    {
        addAndMakeVisible (chooserComponent);

        addAndMakeVisible (okButton);
        okButton.addShortcut (KeyPress (KeyPress::returnKey));

        addAndMakeVisible (cancelButton);
        cancelButton.addShortcut (KeyPress (KeyPress::escapeKey));

        setInterceptsMouseClicks (false, true);
    }

    void paint (Graphics& g) override
    {
        text.draw (g, getLocalBounds().reduced (6)
                        .removeFromTop ((int) text.getHeight()).toFloat());
    }

    void resized() override
    {
        const int buttonHeight = 26;

        auto area = getLocalBounds();

        text.createLayout (getLookAndFeel().createFileChooserHeaderText (getName(), instructions),
                           (float) getWidth() - 12.0f);

        area.removeFromTop (roundToInt (text.getHeight()) + 10);

        chooserComponent.setBounds (area.removeFromTop (area.getHeight() - buttonHeight - 20));
        auto buttonArea = area.reduced (16, 10);

        okButton.changeWidthToFitText (buttonHeight);
        okButton.setBounds (buttonArea.removeFromRight (okButton.getWidth() + 16));

        buttonArea.removeFromRight (16);

        cancelButton.changeWidthToFitText (buttonHeight);
        cancelButton.setBounds (buttonArea.removeFromRight (cancelButton.getWidth()));
    }

    FileBrowserComponent& chooserComponent;
    TextButton okButton, cancelButton;
    String instructions;
    TextLayout text;
};

inf_file_chooser_dialog::inf_file_chooser_dialog(const String& name,
                                            const String& instructions,
                                            FileBrowserComponent& chooserComponent,
                                            bool shouldWarn,
                                            Colour backgroundColour,
                                            Component* parentComp)
    : ResizableWindow (name, backgroundColour, parentComp == nullptr),
      warnAboutOverwritingExistingFiles (shouldWarn)
{
    content = new ContentComponent (name, instructions, chooserComponent);
    setContentOwned (content, false);

    setResizable (true, true);
    setResizeLimits (300, 300, 1200, 1000);

    content->okButton.onClick        = [this] { okButtonPressed(); };
    content->cancelButton.onClick    = [this] { closeButtonPressed(); };

    content->chooserComponent.addListener (this);

    inf_file_chooser_dialog::selectionChanged();

    if (parentComp != nullptr)
        parentComp->addAndMakeVisible (this);
    else
        setAlwaysOnTop (juce_areThereAnyAlwaysOnTopWindows());
}

inf_file_chooser_dialog::~inf_file_chooser_dialog()
{
    content->chooserComponent.removeListener (this);
}

void inf_file_chooser_dialog::centreWithDefaultSize (Component* componentToCentreAround)
{
    centreAroundComponent (componentToCentreAround, getDefaultWidth(), 500);
}

int inf_file_chooser_dialog::getDefaultWidth() const
{
    if (auto* previewComp = content->chooserComponent.getPreviewComponent())
        return 400 + previewComp->getWidth();

    return 600;
}

//==============================================================================
void inf_file_chooser_dialog::closeButtonPressed()
{
    setVisible (false);
}

void inf_file_chooser_dialog::selectionChanged()
{
    content->okButton.setEnabled (content->chooserComponent.currentFileIsValid());
}

void inf_file_chooser_dialog::fileDoubleClicked (const File&)
{
    selectionChanged();
    content->okButton.triggerClick();
}

void inf_file_chooser_dialog::fileClicked (const File&, const MouseEvent&) {}
void inf_file_chooser_dialog::browserRootChanged (const File&) {}

void inf_file_chooser_dialog::okButtonPressed()
{
    
        exitModalState (1);
}

} // namespace inf::base::ui
