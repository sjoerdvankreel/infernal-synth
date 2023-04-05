#include <inf.base.ui/shared/support.hpp>
#include <inf.base.ui/controls/tab_header.hpp>
#include <sstream>

using namespace VSTGUI;
using namespace inf::base;

namespace inf::base::ui {

CView* 
tab_header_creator::create(UIAttributes const& attrs, IUIDescription const* desc) const
{
  std::string const* title;
  title = attrs.getAttributeValue("tab-title");
  assert(title != nullptr);

  std::string item;
  std::vector<std::string> items;
  std::string const* item_list = attrs.getAttributeValue("tab-items");
  assert(item_list != nullptr);
  std::stringstream list_stream(*item_list);
  while(std::getline(list_stream, item, ',')) items.push_back(item);

  tab_header_ui_colors colors;
  colors.back.color = from_vst_color_name(attrs.getAttributeValue("back-color"), desc);
  colors.font.color = from_vst_color_name(attrs.getAttributeValue("font-color"), desc);
  colors.header_font.color = from_vst_color_name(attrs.getAttributeValue("header-font-color"), desc);
  colors.inner_frame.color = from_vst_color_name(attrs.getAttributeValue("inner-frame-color"), desc);
  colors.outer_frame.color = from_vst_color_name(attrs.getAttributeValue("outer-frame-color"), desc);
  colors.active_back.color = from_vst_color_name(attrs.getAttributeValue("active-back-color"), desc);
  colors.active_font.color = from_vst_color_name(attrs.getAttributeValue("active-font-color"), desc);
  return new tab_header(*title, items, colors);
} 
  
tab_header::
tab_header(
  std::string const& title,
  std::vector<std::string> const& items, 
  inf::base::tab_header_ui_colors const& colors):
VSTGUI::CParamDisplay(VSTGUI::CRect(0, 0, 0, 0)), 
_title(title), _items(items), 
_colors(colors), _last_draw_boundaries() 
{ setWantsFocus(true); }

void
tab_header::set_index(std::int32_t val)
{
  float steps = _items.size() - 1.0f;
  setValueNormalized(static_cast<float>(val) / steps);
}

std::int32_t 
tab_header::get_index() const
{
  float steps = _items.size() - 1.0f;
  float max = getValueNormalized() * (steps + 1.0f);
  return static_cast<std::int32_t>(std::min(steps, max));
}

void 
tab_header::onMouseDownEvent(VSTGUI::MouseDownEvent& event)
{
  CParamDisplay::onMouseDownEvent(event);
  for (std::int32_t i = 1; i < static_cast<std::int32_t>(_last_draw_boundaries.size()) - 1; i++)
    if (event.mousePosition.x >= _last_draw_boundaries[i] 
      && event.mousePosition.x <= _last_draw_boundaries[i + 1])
    {
      if(i - 1 != get_index())
      {
        beginEdit();
        set_index(i - 1);
        valueChanged();
        endEdit();
        setDirty(true);
      }
      return;
    }
}

void  
tab_header::draw(CDrawContext* context)
{ 
  double text_y = 15.0;
  double padding = 8.0;
  double title_padding_left = 2.0;
  auto pos = getViewSize().getTopLeft();
  double current_left = getViewSize().left;
  CDrawContext::Transform transform(*context, CGraphicsTransform().translate(pos));

  context->setLineWidth(1.0);        
  context->setFont(getFont(), 11);
  context->setDrawMode(kAntialias);    
  context->setFrameColor(to_vst_color(_colors.inner_frame.color));
  _last_draw_boundaries.clear();
  _last_draw_boundaries.push_back(current_left);

  // Title 
  context->setFontColor(to_vst_color(_colors.header_font.color));
  context->setFillColor(to_vst_color(_colors.active_back.color));
  double title_width = context->getStringWidth(_title.c_str());
  current_left += title_width + title_padding_left + padding;
  _last_draw_boundaries.push_back(current_left);
  auto title_box = CRect(getViewSize().left, getViewSize().top, current_left, getViewSize().bottom - 1);
  context->drawRect(title_box, kDrawFilled);
  context->drawString(_title.c_str(), CPoint(getViewSize().left + title_padding_left, text_y));

  // Separators and items 
  for(std::int32_t i = 0; i < static_cast<std::int32_t>(_items.size()); i++)
  { 
    context->setFontColor(to_vst_color(i == get_index() ? _colors.active_font.color : _colors.font.color));
    context->setFillColor(to_vst_color(i == get_index() ? _colors.active_back.color : _colors.back.color));
    double text_width = context->getStringWidth(_items[i].c_str());
    double cell_width = text_width + 2.0 * padding; 
    auto line_start = CPoint(current_left, pos.y);
    auto line_end = CPoint(current_left, pos.y + getViewSize().getHeight());
    auto cell_size = CPoint(cell_width, getViewSize().getHeight());
    context->drawRect(CRect(line_start, cell_size), kDrawFilled);
    context->drawLine(line_start, line_end);
    context->drawString(_items[i].c_str(), CPoint(current_left + padding, text_y));
    current_left += cell_width;
    _last_draw_boundaries.push_back(current_left);
  } 

  // Outer frame
  context->setFrameColor(to_vst_color(_colors.outer_frame.color));
  context->drawLine(CPoint(current_left, pos.y), CPoint(current_left, pos.y + getViewSize().getHeight()));
  setDirty(false);
}
 
} // namespace inf::base::ui
 