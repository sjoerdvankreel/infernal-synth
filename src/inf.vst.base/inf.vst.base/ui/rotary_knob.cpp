#include <inf.vst.base/ui/support.hpp>
#include <inf.base/shared/support.hpp>
#include <inf.vst.base/ui/rotary_knob.hpp>
#include <vstgui/uidescription/uiviewcreator.h>
#include <cassert>

using namespace VSTGUI;
using namespace inf::base;

namespace inf::vst::base {

static CPoint
point_on_circle(
  double center, double radius, double theta)
{
  double x = radius * std::sin(theta) + center;
  double y = radius * std::cos(theta) + center;
  return CPoint(x, y);
}

CView* 
rotary_knob_creator::create(
  UIAttributes const& attrs, IUIDescription const* desc) const
{  
  bool ok;
  bool bipolar;
  bool discrete;
  knob_ui_colors colors;  

  ok = attrs.getBooleanAttribute("bipolar", bipolar);
  assert(ok);
  (void)ok;
  ok = attrs.getBooleanAttribute("discrete", discrete);
  assert(ok);
  (void)ok;

  colors.fill = from_vst_color_name(attrs.getAttributeValue("fill-color"), desc);
  colors.drag = from_vst_color_name(attrs.getAttributeValue("drag-color"), desc);
  colors.marker = from_vst_color_name(attrs.getAttributeValue("marker-color"), desc);
  colors.inner = from_vst_color_name(attrs.getAttributeValue("inner-color"), desc);
  colors.outer = from_vst_color_name(attrs.getAttributeValue("outer-color"), desc);
  colors.light = from_vst_color_name(attrs.getAttributeValue("light-color"), desc);
  colors.shadow = from_vst_color_name(attrs.getAttributeValue("shadow-color"), desc);
  return new rotary_knob(colors, bipolar, discrete);
} 

void
rotary_knob::draw(VSTGUI::CDrawContext* context)
{
  float start = getStartAngle();
  float range = getRangeAngle();
  float angle = getValueNormalized();
  float to_degrees = 360.0f / (2.0f * inf::base::pi32);

  auto pos = getViewSize().getTopLeft() + CPoint(1.0, 1.0);
  CDrawContext::Transform transform(*context, CGraphicsTransform().translate(pos));

  // correct     
  auto outer_size = getViewSize().getSize();
  auto inner_size = CPoint({ outer_size.x - 2.0, outer_size.y - 2.0 });
  if (static_cast<std::int32_t>(std::ceil(inner_size.x)) % 2 == 0) inner_size.x -= 1.0f;
  if (static_cast<std::int32_t>(std::ceil(inner_size.y)) % 2 == 0) inner_size.y -= 1.0f;

  // fill
  context->setDrawMode(kAntiAliasing); 
  context->setFillColor(to_vst_color(_colors.fill));
  context->drawEllipse(CRect(CPoint(1, 1), inner_size - CPoint(2, 2)), kDrawFilled);

  // outer border
  context->setFrameColor(to_vst_color(_colors.outer));
  context->drawEllipse(CRect(CPoint(1, 1), inner_size - CPoint(2, 2)), kDrawStroked);

  // inner border  
  if(_discrete)
  {
    float border_hi_start = 90.0f + start * to_degrees + angle * range * to_degrees + 45.0f;
    float border_hi_end = border_hi_start + 270.0f;
    context->setFrameColor(to_vst_color(_colors.drag));
    context->drawArc(CRect(CPoint(3, 3), inner_size - CPoint(6, 6)), border_hi_start, border_hi_end, kDrawStroked);
    context->setFrameColor(to_vst_color(_colors.inner.darken(0.5f)));
    context->drawArc(CRect(CPoint(3, 3), inner_size - CPoint(6, 6)), border_hi_end, border_hi_start, kDrawStroked);
  }
  else if (!_bipolar)
  {
    float border_hi_start = 90.0f + start * to_degrees;
    float border_hi_end = 90.0f + start * to_degrees + angle * range * to_degrees;
    float border_lo_end = 90.0f - start * to_degrees;
    context->setFrameColor(to_vst_color(_colors.drag));
    context->drawArc(CRect(CPoint(3, 3), inner_size - CPoint(6, 6)), border_hi_start, border_hi_end, kDrawStroked);
    context->setFrameColor(to_vst_color(_colors.inner));
    context->drawArc(CRect(CPoint(3, 3), inner_size - CPoint(6, 6)), border_hi_end, border_lo_end, kDrawStroked);
    context->setFrameColor(to_vst_color(_colors.inner));
    context->drawArc(CRect(CPoint(3, 3), inner_size - CPoint(6, 6)), border_lo_end, border_hi_start, kDrawStroked);
  } else if(angle >= 0.5f)
  {
    float border_lo_start_1 = 90.0f + start * to_degrees;
    float border_hi_start = 270.0f;
    float border_hi_end = 270.0f + (angle - 0.5f) * range * to_degrees;
    float border_lo_end_2 = 90.0f - start * to_degrees;
    context->setFrameColor(to_vst_color(_colors.inner));
    context->drawArc(CRect(CPoint(3, 3), inner_size - CPoint(6, 6)), border_lo_start_1, border_hi_start, kDrawStroked);
    context->setFrameColor(to_vst_color(_colors.drag));
    context->drawArc(CRect(CPoint(3, 3), inner_size - CPoint(6, 6)), border_hi_start, border_hi_end, kDrawStroked);
    context->setFrameColor(to_vst_color(_colors.inner));
    context->drawArc(CRect(CPoint(3, 3), inner_size - CPoint(6, 6)), border_hi_end, border_lo_end_2, kDrawStroked);
    context->setFrameColor(to_vst_color(_colors.inner));
    context->drawArc(CRect(CPoint(3, 3), inner_size - CPoint(6, 6)), border_lo_end_2, border_lo_start_1, kDrawStroked);
  }
  else 
  {
    float border_hi_start = 270.0f - (0.5f - angle) * range * to_degrees;
    float border_hi_end = 270.0f;
    //context->setFrameColor(to_vst_color(_colors.drag));
    context->setFrameColor(CColor(255, 0, 0));
    context->drawArc(CRect(CPoint(3, 3), inner_size - CPoint(6, 6)), border_hi_start, border_hi_end, kDrawStroked);
    //context->setFrameColor(to_vst_color(_colors.inner));
    context->setFrameColor(CColor(0, 255, 0));
    context->drawArc(CRect(CPoint(3, 3), inner_size - CPoint(6, 6)), border_hi_end, border_hi_start, kDrawStroked);
  }

  // light settings
  std::int32_t light_parts = 15;
  std::int32_t light_top_index = light_parts / 2;

  // highlight
  std::int32_t highlight_max_alpha = 192;
  std::int32_t highlight_part_alpha_contrib = highlight_max_alpha / ((light_parts + 1) / 2);
  for (std::int32_t i = 0; i < light_parts; i++)
  {
    std::int32_t alpha_index = i <= light_top_index ? i: light_top_index - (i - light_top_index);
    float angle1 = 180.0f + i * 90.0f / light_parts;
    float angle2 = 180.0f + (i + 1) * 90.0f / light_parts;
    std::uint8_t alpha = static_cast<std::uint8_t>((alpha_index + 1) * highlight_part_alpha_contrib);
    context->setFrameColor(CColor(_colors.light.r, _colors.light.g, _colors.light.b, alpha));
    context->drawArc(CRect(CPoint(1, 1), inner_size - CPoint(2, 2)), angle1, angle2, kDrawStroked);
  }

  // shadow
  std::int32_t shadow_max_alpha = 255;
  std::int32_t shadow_part_alpha_contrib = shadow_max_alpha / ((light_parts + 1) / 2);
  for (std::int32_t i = 0; i < light_parts; i++)
  {
    std::int32_t alpha_index = i <= light_top_index ? i : light_top_index - (i - light_top_index);
    float angle1 = i * 90.0f / light_parts;
    float angle2 = (i + 1) * 90.0f / light_parts;
    std::uint8_t alpha = static_cast<std::uint8_t>((alpha_index + 1) * shadow_part_alpha_contrib);
    context->setFrameColor(CColor(_colors.shadow.r, _colors.shadow.g, _colors.shadow.b, alpha));
    context->drawArc(CRect(CPoint(1, 1), inner_size - CPoint(2, 2)), angle1, angle2, kDrawStroked);
  }

  // marker
  double center = (inner_size.x - 1.0) / 2.0;
  double radius = center - 3.0;
  double theta = -(start + angle * range);
  context->setFrameColor(to_vst_color(_colors.marker));
  context->drawLine(CPoint(center, center), point_on_circle(center, radius, theta));

  // spot markers
  if (_bipolar)
  {
    context->setFillColor(to_vst_color(_colors.inner));
    context->drawEllipse(CRect(CPoint(outer_size.x / 2.0 - 3.0, 1.0), CPoint(3.0, 3.0)), kDrawFilled);
  }
  else if(!_discrete)
  {
    context->setFillColor(to_vst_color(angle >= 0.99f ? _colors.marker : _colors.inner));
    context->drawEllipse(CRect(point_on_circle(center, radius, start), CPoint(2.0, 2.0)), kDrawFilled);
    context->setFillColor(to_vst_color(angle >= 0.01f ? _colors.inner : _colors.marker));
    context->drawEllipse(CRect(point_on_circle(center, radius, -start), CPoint(2.0, 2.0)), kDrawFilled);
  }
} 

} // namespace inf::vst::base