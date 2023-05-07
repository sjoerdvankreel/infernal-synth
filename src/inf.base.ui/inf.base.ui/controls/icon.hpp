#ifndef INF_BASE_UI_CONTROLS_ICON_HPP
#define INF_BASE_UI_CONTROLS_ICON_HPP

#include <inf.base.ui/shared/support.hpp>
#include <juce_gui_basics/juce_gui_basics.h>
#include <cstdint>

namespace inf::base::ui {

typedef icon_type(*icon_selector)(std::int32_t value);

class inf_icon:
public juce::Component
{
  std::int32_t _value;
  icon_selector const _selector;
public:
  void paint(juce::Graphics& g) override;
  void value(std::int32_t v) { _value = v; }
  std::int32_t value() const { return _value; }
  inf_icon(std::int32_t value, icon_selector selector): _value(value), _selector(selector) {}
};

} // namespace inf::base::ui
#endif // INF_BASE_UI_CONTROLS_ICON_HPP