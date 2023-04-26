#ifndef INF_BASE_UI_CONTROLS_CONTAINER_HPP
#define INF_BASE_UI_CONTROLS_CONTAINER_HPP

#include <juce_gui_basics/juce_gui_basics.h>

namespace inf::base::ui {

class container_component:
public juce::Component
{
  float const _radius;
  float const _thickness;
  juce::Colour const _fill;
  juce::Colour const _outline;
  std::uint32_t const _flags;
public:
  enum flags { none = 0x0, fill = 0x1, outline = 0x2 };
  void paint(juce::Graphics& g) override;
  container_component(std::uint32_t flags, float radius, float thickness, juce::Colour const& fill, juce::Colour const& outline): 
  _radius(radius), _thickness(thickness), _fill(fill), _outline(outline), _flags(flags) {}
};

} // namespace inf::base::ui
#endif // INF_BASE_UI_CONTROLS_CONTAINER_HPP