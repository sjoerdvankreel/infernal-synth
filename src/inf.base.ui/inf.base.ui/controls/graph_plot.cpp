#include <inf.base.ui/shared/support.hpp>
#include <inf.base.ui/controls/graph_plot.hpp>
#include <inf.base/plugin/plugin_controller.hpp>
#include <inf.base/plugin/graph_processor.hpp>
#include <inf.base/topology/part_ui_descriptor.hpp>

#include <vstgui/uidescription/uiviewcreator.h>

#include <cassert>
#include <cstdint>

using namespace VSTGUI;
using namespace inf::base;
   
namespace inf::base::ui {
      
CView*  
graph_plot_creator::create(
  UIAttributes const& attrs, IUIDescription const* desc) const
{    
  bool ok;
  graph_ui_colors colors;
  std::int32_t row_span;
  std::int32_t part_type; 
  std::int32_t graph_type;
  std::int32_t part_index;
  std::int32_t column_span;
  std::string const* tooltip; 
        
  ok = attrs.getIntegerAttribute("row-span", row_span);
  assert(ok);
  ok = attrs.getIntegerAttribute("part-type", part_type);
  assert(ok);
  ok = attrs.getIntegerAttribute("graph-type", graph_type);
  assert(ok);
  ok = attrs.getIntegerAttribute("part-index", part_index);
  assert(ok);
  ok = attrs.getIntegerAttribute("column-span", column_span);
  assert(ok);
  (void)ok;

  colors.line.color = from_vst_color_name(attrs.getAttributeValue("line-color"), desc);
  colors.grid.color = from_vst_color_name(attrs.getAttributeValue("grid-color"), desc);
  colors.area.color = from_vst_color_name(attrs.getAttributeValue("area-color"), desc);
  colors.fill.color = from_vst_color_name(attrs.getAttributeValue("fill-color"), desc);
  colors.frame.color = from_vst_color_name(attrs.getAttributeValue("frame-color"), desc);

  part_id id = { part_type, part_index };
  tooltip = attrs.getAttributeValue("tooltip");
  graph_plot* result = new graph_plot(id, graph_type, colors, row_span, column_span);
  if(tooltip != nullptr) result->setTooltipText(tooltip->data());
  return result;
} 

void
graph_plot::draw(VSTGUI::CDrawContext* context)
{
  std::int32_t const padx = 2;
  std::int32_t const pady = 3;

  auto size = getViewSize().getSize();
  auto pos = getViewSize().getTopLeft();
  CPoint render_size(size.x - padx * 2, size.y - pady * 2);
  CDrawContext::Transform transform(*context, CGraphicsTransform().translate(pos));
  
  // Fill.
  context->setFillColor(to_vst_color(_colors.fill.color));
  context->drawRect(CRect(CPoint(0, 0), size), CDrawStyle::kDrawFilled);
        
  // Frame.
  context->setLineWidth(1.0);
  context->setDrawMode(kAntiAliasing);
  context->setFrameColor(to_vst_color(_colors.frame.color));
  context->drawRect(CRect(CPoint(0, 0), size), CDrawStyle::kDrawStroked);

  // Grid. 
  context->setLineWidth(1.0);
  context->setDrawMode(kAntiAliasing); 
  context->setFrameColor(to_vst_color(_colors.grid.color));
  std::int32_t const h_segment_count = 8 * _column_span;
  double segment_width = size.x / h_segment_count;
  for (std::int32_t i = 1; i < h_segment_count; i++)
    context->drawLine(CPoint(i * segment_width, 1), CPoint(i * segment_width, size.y - 1));  
  std::int32_t const v_segment_count = 4 * _row_span;
  double segment_height = size.y / v_segment_count;
  for (std::int32_t i = 1; i < v_segment_count; i++)
    context->drawLine(CPoint(1, i * segment_height), CPoint(size.x - 1, i * segment_height));
   
  bool bipolar;
  param_value const* state = find_editor(this)->state();
  std::vector<graph_point> const& graph_data = _processor->plot(
    state, sample_rate,
    static_cast<std::int32_t>(render_size.x), 
    static_cast<std::int32_t>(render_size.y), 
    bipolar);
  float opacity = _processor->opacity(state);
  if(graph_data.size() == 0) return;
    
  // Area.
  double max_y = 0.0;
  double min_y = size.y;
  double base_y = bipolar? 0.5: 1.0;
  CGraphicsPath* fill_path = context->createGraphicsPath(); 
  fill_path->beginSubpath(CPoint(graph_data[0].x + padx / 2, render_size.y * base_y + pady / 2 + 2.0));
  for(std::size_t i = 0; i < graph_data.size(); i++)
  {
    fill_path->addLine(graph_data[i].x + padx / 2, render_size.y - graph_data[i].y + pady / 2 + 2.0);
    min_y = std::min(min_y, static_cast<double>(graph_data[i].y));
    max_y = std::max(max_y, static_cast<double>(graph_data[i].y));
  }
  fill_path->addLine(render_size.x + padx / 2, render_size.y * base_y + pady / 2 + 2.0);
  GradientColorStopMap unipolar_map({
    { 0.0, to_vst_color(_colors.area.color.opacity(opacity)) },
    { 1.0, to_vst_color(_colors.area.color.opacity(opacity * 0.1f)) } });
  GradientColorStopMap bipolar_map({
    { 0.0, to_vst_color(_colors.area.color.opacity(opacity)) },
    { 0.5, to_vst_color(_colors.area.color.opacity(opacity * 0.1f)) },
    { 1.0, to_vst_color(_colors.area.color.opacity(opacity)) } });
  CGradient* gradient = CGradient::create(bipolar? bipolar_map: unipolar_map);
  if(bipolar)
  {
    double range = std::min(min_y, size.y - max_y);
    context->fillLinearGradient(fill_path, *gradient, CPoint(0.0, range), CPoint(0.0, size.y - range));
  }
  else
    context->fillLinearGradient(fill_path, *gradient, CPoint(0.0, size.y - max_y), CPoint(0.0, size.y));
  fill_path->forget();
  gradient->forget();

  // Line.
  context->setLineWidth(0.75);
  context->setDrawMode(kAntiAliasing);
  context->setFrameColor(to_vst_color(_colors.line.color.opacity(opacity)));
  CGraphicsPath* stroke_path = context->createGraphicsPath();
  stroke_path->beginSubpath(CPoint(graph_data[0].x + padx, render_size.y - graph_data[0].y + pady));
  for (std::size_t i = 1; i < graph_data.size(); i++)
    stroke_path->addLine(graph_data[i].x + padx, render_size.y - graph_data[i].y + pady);
  context->drawGraphicsPath(stroke_path, CDrawContext::kPathStroked);
  stroke_path->forget();
}

} // namespace inf::base::ui