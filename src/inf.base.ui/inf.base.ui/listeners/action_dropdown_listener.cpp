#include <inf.base.ui/listeners/action_dropdown_listener.hpp>

namespace inf::base::ui {

void 
action_dropdown_listener::comboBoxChanged(juce::ComboBox* dropdown) 
{
  _callback(dropdown->getSelectedItemIndex()); 
  dropdown->setSelectedId(0, juce::dontSendNotification);
}

} // namespace inf::base::ui