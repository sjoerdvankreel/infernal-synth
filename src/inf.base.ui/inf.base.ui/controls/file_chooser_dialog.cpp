#include <inf.base.ui/controls/button.hpp>
#include <inf.base.ui/controls/file_chooser_dialog.hpp>

using namespace juce;

namespace inf::base::ui {

class inf_file_chooser_dialog::content_component:
public Component
{
public:
  inf_button ok;
  inf_button cancel;
  TextLayout text = {};
  FileBrowserComponent& chooser_component;

  content_component(
    inf::base::plugin_controller* controller, 
    String const& name, FileBrowserComponent& chooser);
  
  void resized() override;
  void paint(Graphics& g) override
  { text.draw(g, getLocalBounds().reduced(6).removeFromTop(static_cast<int>(text.getHeight())).toFloat()); }
};

inf_file_chooser_dialog::content_component::
content_component(
  inf::base::plugin_controller* controller,
  String const& name, FileBrowserComponent& chooser) :
  Component(name), ok(controller), cancel(controller), chooser_component(chooser)
{
  ok.setButtonText("OK");
  cancel.setButtonText("Cancel");
  addAndMakeVisible(chooser_component);
  addAndMakeVisible(ok);
  ok.addShortcut(KeyPress(KeyPress::returnKey));
  addAndMakeVisible(cancel);
  cancel.addShortcut(KeyPress(KeyPress::escapeKey));
  setInterceptsMouseClicks(false, true);
}

void 
inf_file_chooser_dialog::content_component::resized()
{
  const int button_height = 20;
  auto area = getLocalBounds();
  text.createLayout(getLookAndFeel().createFileChooserHeaderText(
    getName(), String()), static_cast<float>(getWidth()) - 12.0f);
  area.removeFromTop(roundToInt(text.getHeight()) + 10);
  chooser_component.setBounds(area.removeFromTop(area.getHeight() - button_height - 20));
  cancel.changeWidthToFitText(button_height);
  cancel.setBounds(area.removeFromRight(cancel.getWidth()));
  ok.changeWidthToFitText(button_height);
  ok.setBounds(area.removeFromRight(ok.getWidth()));
}

inf_file_chooser_dialog::
inf_file_chooser_dialog(
  inf::base::plugin_controller* controller, juce::String const& title,
  juce::FileBrowserComponent& browser_component,
  juce::Colour background_colour, juce::Component* parent_component):
ResizableWindow (title, background_colour, false), _controller(controller)
{
  content = new content_component(controller, title, browser_component);
  setContentOwned(content, false);
  setResizable(false, false);
  content->ok.onClick = [this] { exitModalState(1); };
  content->cancel.onClick = [this] { setVisible(false); };
  content->chooser_component.addListener(this);
  inf_file_chooser_dialog::selectionChanged();
  parent_component->addAndMakeVisible (this);
}

inf_file_chooser_dialog::~inf_file_chooser_dialog()
{ content->chooser_component.removeListener(this); }
void inf_file_chooser_dialog::selectionChanged()
{ content->ok.setEnabled(content->chooser_component.currentFileIsValid()); }

void inf_file_chooser_dialog::fileDoubleClicked(File const&)
{
  selectionChanged();
  content->ok.triggerClick();
}

} // namespace inf::base::ui
