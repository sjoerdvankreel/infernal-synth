#ifndef INF_BASE_UI_CONTROLS_CONTAINER_HPP
#define INF_BASE_UI_CONTROLS_CONTAINER_HPP

#include <juce_gui_basics/juce_gui_basics.h>
#include <cstdint>

namespace inf::base::ui {

class container_component:
public juce::Component
{
  float const _radius;
  float const _thickness;
  std::uint32_t const _flags;
  std::int32_t const _fill_low_color_id;
  std::int32_t const _fill_high_color_id;
  std::int32_t const _outline_low_color_id;
  std::int32_t const _outline_high_color_id;
public:
  enum flags { none = 0x0, fill = 0x1, outline = 0x2, both = fill | outline };
  void paint(juce::Graphics& g) override;
  container_component(
    std::uint32_t flags, float radius, float thickness, 
    std::int32_t fill_low_color_id, std::int32_t fill_high_color_id, 
    std::int32_t outline_low_color_id, std::int32_t outline_high_color_id):
  _radius(radius), _thickness(thickness), _flags(flags), 
  _fill_low_color_id(fill_low_color_id), _fill_high_color_id(fill_high_color_id),
  _outline_low_color_id(outline_low_color_id), _outline_high_color_id(outline_high_color_id) {}
};

} // namespace inf::base::ui
#endif // INF_BASE_UI_CONTROLS_CONTAINER_HPP