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
  auto bounds = with_container_padding(getLocalBounds()).toFloat();
  if (_flags & flags::fill)
  {
    g.setColour(findColour(_fill_color_id));
    g.fillRoundedRectangle(bounds, _radius);
  }
  if (_flags & flags::outline)
  {
    g.setColour(findColour(_outline_color_id));
    g.drawRoundedRectangle(bounds, _radius, _thickness);
  }
  if(getChildren().size() != 0)
    getChildComponent(0)->paint(g);
}

} // namespace inf::base::ui