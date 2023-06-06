#include <inf.base.ui/controls/file_chooser_dialog.hpp>

using namespace juce;

namespace inf::base::ui {

class inf_file_chooser_dialog::ContentComponent  : public Component
{
public:
    ContentComponent (const String& name, FileBrowserComponent& chooser)
        : Component (name),
          chooserComponent (chooser),
          okButton (chooser.getActionVerb()),
          cancelButton (TRANS ("Cancel"))
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
                                            FileBrowserComponent& chooserComponent,
                                            juce::Colour backgroundColour,
                                            Component* parentComp)
    : ResizableWindow (name, backgroundColour, parentComp == nullptr)
{
    content = new ContentComponent (name, chooserComponent);
    setContentOwned (content, false);

    setResizable (true, true);
    setResizeLimits (300, 300, 1200, 1000);

    content->okButton.onClick        = [this] { exitModalState(1); };
    content->cancelButton.onClick    = [this] { setVisible(false); };

    content->chooserComponent.addListener (this);

    inf_file_chooser_dialog::selectionChanged();

        parentComp->addAndMakeVisible (this);
}

inf_file_chooser_dialog::~inf_file_chooser_dialog()
{
    content->chooserComponent.removeListener (this);
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

} // namespace inf::base::ui
