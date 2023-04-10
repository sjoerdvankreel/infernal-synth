#ifndef INF_BASE_UI_SUPPORT_HPP
#define INF_BASE_UI_SUPPORT_HPP

#include <juce_gui_basics/juce_gui_basics.h>

namespace inf::base::ui {

void
recursive_destroy_children(juce::Component* component);

} // namespace inf::base::ui
#endif INF_BASE_UI_SUPPORT_HPP