#ifndef INF_BASE_UI_SUPPORT_HPP
#define INF_BASE_UI_SUPPORT_HPP

#include <juce_gui_basics/juce_gui_basics.h>
#include <vector>
#include <memory>

namespace inf::base::ui {

class juce_gui_state
{
  std::vector<std::unique_ptr<juce::Component>> _components;
public:
  void clear() { _components.clear(); }
  template <class T, class... Args> T* create(Args&&... args);
};

template <class T, class... Args>
T* juce_gui_state::create(Args&&... args)
{
  std::unique_ptr<T> component(std::make_unique<T>(std::forward<Args>(args)...));
  T* result = component.get();
  _components.push_back(std::move(component));
  return result;
}

} // namespace inf::base::ui
#endif INF_BASE_UI_SUPPORT_HPP