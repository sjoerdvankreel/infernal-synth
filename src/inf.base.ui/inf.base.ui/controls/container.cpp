#include <inf.base.ui/controls/container.hpp>
#include <inf.base.ui/shared/look_and_feel.hpp>

using namespace juce;
using namespace inf::base;

static int const container_padding = 2;

static Rectangle<int> 
with_container_padding(Rectangle<int> const& bounds)
{
  return Rectangle<int>(
    bounds.getX() + container_padding, bounds.getY() + container_padding,
    bounds.getWidth() - 2 * container_padding, bounds.getHeight() - 2 * container_padding);
}

namespace inf::base::ui {

void 
container_component::paint(Graphics& g)
{
  auto& lnf = dynamic_cast<inf_look_and_feel&>(getLookAndFeel());
  auto bounds = with_container_padding(getLocalBounds()).toFloat();
#if 0  
  if (_flags & flags::fill)
    lnf.fill_gradient_rounded_rectangle(
      g, *this, bounds, inf_look_and_feel::colors::part_group_container_fill_low, 
      inf_look_and_feel::colors::part_group_container_fill_high, _radius, 0.5f); 
  if (_flags & flags::outline)
    lnf.stroke_gradient_rounded_rectangle(
      g, *this, bounds, inf_look_and_feel::colors::part_group_container_outline_low,
      inf_look_and_feel::colors::part_group_container_outline_high, _radius, 0.5f, _thickness);
#endif
  if(getChildren().size() != 0)
    getChildComponent(0)->paint(g);
}

} // namespace inf::base::ui