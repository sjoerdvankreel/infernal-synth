#include <inf.base.ui/controls/container.hpp>
#include <inf.base.ui/shared/look_and_feel.hpp>

using namespace juce;
using namespace inf::base;

namespace inf::base::ui {

void 
container_component::paint(Graphics& g)
{
  auto& lnf = dynamic_cast<inf_look_and_feel&>(getLookAndFeel());
  auto bounds = getLocalBounds().expanded(-container_margin, -container_margin).toFloat();
  if (_flags & flags::fill)
    lnf.fill_gradient_rounded_rectangle(
      g, *this, bounds, _fill_low_color_id, _fill_high_color_id, _radius, 0.5f); 
  if (_flags & flags::outline)
    lnf.stroke_gradient_rounded_rectangle(
      g, *this, bounds, _outline_low_color_id, _outline_high_color_id, _radius, 0.5f, _thickness);
  if(getChildren().size() != 0)
    getChildComponent(0)->paint(g);
}

} // namespace inf::base::ui