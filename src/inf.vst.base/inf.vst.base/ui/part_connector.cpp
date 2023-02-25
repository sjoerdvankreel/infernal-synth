#include <inf.vst.base/ui/support.hpp>
#include <inf.vst.base/ui/part_connector.hpp>
#include <vstgui/uidescription/uiviewcreator.h>

#include <cassert>
#include <cstdint>

using namespace VSTGUI;
using namespace inf::base;
   
namespace inf::vst::base {
      
CView*  
part_connector_creator::create(
  UIAttributes const& attrs, IUIDescription const* desc) const
{    
  bool ok;
  std::int32_t direction;
  ok = attrs.getIntegerAttribute("connector-direction", direction);
  assert(ok);
  (void)ok;
  ui_color color = from_vst_color_name(attrs.getAttributeValue("connector-color"), desc);
  return new part_connector(static_cast<connector_direction>(direction), color);
} 

void
part_connector::draw(VSTGUI::CDrawContext* context)
{
  auto size = getViewSize().getSize().x;
  auto pos = getViewSize().getTopLeft();
  CDrawContext::Transform transform(*context, CGraphicsTransform().translate(pos));

  context->setDrawMode(kAliasing);
  context->setFillColor(to_vst_color(_color));
  auto path = context->createGraphicsPath();
   
  switch(_direction)  
  {
  case connector_direction::up:
    path->beginSubpath(size / 6.0, size);
    path->addLine(size * 0.5, size * 2.0 / 3.0);
    path->addLine(size * 5.0 / 6.0, size);
    path->closeSubpath();
    break;
  case connector_direction::down:
    path->beginSubpath(size / 6.0, 0.0);
    path->addLine(size * 5.0 / 6.0, 0.0);
    path->addLine(size * 0.5, size / 3.0);
    path->closeSubpath();
    break;  
  case connector_direction::left:
    path->beginSubpath(size * 7.0 / 12.0 + 1.0, size / 2.0);
    path->addLine(size * 11.0 / 12.0, size / 5.0);
    path->addLine(size * 11.0 / 12.0, size * 4.0 / 5.0);
    path->closeSubpath();
    break;
  case connector_direction::right:
    path->beginSubpath(size * 7.0 / 12.0 + 1.0, size / 4.0);
    path->addLine(size * 11.0 / 12.0, size / 2.0);
    path->addLine(size * 7.0 / 12.0 + 1.0, size * 3.0 / 4.0);
    path->closeSubpath();
    break; 
  case connector_direction::halfway_right_down:
    break; 
  case connector_direction::left_right:
    path->beginSubpath(size * 2.0 / 12.0 + 1.0, size / 4.0);
    path->addLine(size * 6.0 / 12.0, size / 2.0);
    path->addLine(size * 2.0 / 12.0 + 1.0, size * 3.0 / 4.0);
    path->closeSubpath(); 
    path->beginSubpath(size * 7.0 / 12.0 + 1.0, size / 2.0);
    path->addLine(size * 11.0 / 12.0, size / 5.0);
    path->addLine(size * 11.0 / 12.0, size * 4.0 / 5.0);
    path->closeSubpath();
    break;  
  default:
    assert(false);    
    break;
  }

  context->drawGraphicsPath(path);
  path->forget();
}

} // namespace inf::vst::base