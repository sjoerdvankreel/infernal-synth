#include <inf.base.ui/support.hpp>
#include <cassert>

namespace inf::base::ui {

void
recursive_destroy_children(juce::Component* component)
{
  assert(component != nullptr);
  while(component->getNumChildComponents() > 0)
    recursive_destroy_children(component->getChildComponent(0));
}

} // namespace inf::base::ui