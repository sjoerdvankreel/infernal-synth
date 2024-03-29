#include <inf.base/shared/support.hpp>
#include <inf.base.ui/shared/look_and_feel.hpp>
#include <inf.base.ui/controls/selector_label.hpp>

using namespace juce;

namespace inf::base::ui {

void 
inf_selector_label::mouseUp(MouseEvent const& event)
{
  if(_part_type == -1 || !event.mods.isRightButtonDown()) return;
  PopupMenu menu;
  menu.setLookAndFeel(&getLookAndFeel());
  menu.addItem(1, _part_count == 1? "Clear": "Clear all");
  menu.showMenuAsync(PopupMenu::Options(), [this](int option)
  {
    if (option == 0) return;
    for (int i = 0; i < _part_count; i++)
      _controller->clear_part({ _part_type, i });
  });
}

void
inf_selector_label::paint(Graphics& g)
{
  // config
  int const vpad = 2;
  int const vhpad = 3;
  int const hpadl = 1;
  int const hpadr = _vertical? 1: 3;
  float const corner_size_fixed = 5.0f;
  float const outline_size_fixed = 1.0f;
  float const router_hmargin = 4.0f;
  float const router_vmargin = 1.0f;
  float const router_width = get_router_width(_controller);
  float const router_line_size = get_router_line_size(_controller);
  float const router_arrow_size = get_router_arrow_size(_controller);
  int const text_padl = _justification == Justification::left ? 5 : 0;
  int const text_padr = _justification == Justification::right ? 5 : 0;

  auto& lnf = dynamic_cast<inf_look_and_feel&>(getLookAndFeel());
  auto area = Rectangle<int>(
    getLocalBounds().getX() + hpadl,
    getLocalBounds().getY() + vpad / 2,
    getLocalBounds().getWidth() - hpadl - hpadr,
    getLocalBounds().getHeight() - vpad);

  // fill
  std::int32_t background_low = inf_look_and_feel::colors::selector_label_background_low;
  std::int32_t background_high = inf_look_and_feel::colors::selector_label_background_high;
  lnf.fill_gradient_rounded_rectangle(
    g, *this, area.toFloat(), background_low,
    background_high, corner_size_fixed, 0.25f);

  // outline
  lnf.stroke_gradient_rounded_rectangle(
    g, *this, area.toFloat(), inf_look_and_feel::colors::selector_label_outline_low,
    inf_look_and_feel::colors::selector_label_outline_high, corner_size_fixed, 0.25f, outline_size_fixed);

  // text
  auto text_area = Rectangle<int>(area.getX() + text_padl, area.getY(), area.getWidth() - text_padl - text_padr, area.getHeight());

  g.setFont(getFont());
  g.setColour(findColour(inf_look_and_feel::colors::selector_label_text));
  g.saveState();
  if(_vertical) 
  {
    auto transform = AffineTransform().rotated(-pi32 / 2.0f, text_area.getWidth() / 2.0f, text_area.getHeight() / 2.0f);
    g.addTransform(transform);
    text_area = text_area.transformedBy(transform);
    text_area = juce::Rectangle<int>(text_area.getX() - vhpad, text_area.getY() + vhpad, text_area.getWidth() + vhpad, text_area.getHeight() - vhpad);
  }
  g.drawText(getText(), text_area, _justification, false);

  float router_l_left = router_hmargin;
  float router_l_right = router_hmargin + router_width;
  float router_r_right = area.getWidth() - router_hmargin;
  float router_r_left = area.getWidth() - router_hmargin - router_width;
  float router_y = area.getHeight() / 2.0f + router_vmargin;
  switch (_routing_dir)
  {
  case selector_routing_dir::none:
    break;
  case selector_routing_dir::left_toleft:
    g.drawArrow(juce::Line<float>(router_l_right, router_y, router_l_left, router_y), router_line_size, router_arrow_size, router_arrow_size);
    break;
  case selector_routing_dir::left_toright:
    g.drawArrow(juce::Line<float>(router_l_left, router_y, router_l_right, router_y), router_line_size, router_arrow_size, router_arrow_size);
    break;
  case selector_routing_dir::left_bidirectional:
    g.drawArrow(juce::Line<float>(router_l_left, router_y, router_l_right, router_y), router_line_size, router_arrow_size, router_arrow_size);
    g.drawArrow(juce::Line<float>(router_l_right + router_width, router_y, router_l_left + router_width, router_y), router_line_size, router_arrow_size, router_arrow_size);
    break;
  case selector_routing_dir::right_toleft:
    g.drawArrow(juce::Line<float>(router_r_right, router_y, router_r_left, router_y), router_line_size, router_arrow_size, router_arrow_size);
    break;
  case selector_routing_dir::right_toright:
    g.drawArrow(juce::Line<float>(router_r_left, router_y, router_r_right, router_y), router_line_size, router_arrow_size, router_arrow_size);
    break;
  default:
    assert(false);
    break;
  }
  g.restoreState();
}

} // namespace inf::base::ui