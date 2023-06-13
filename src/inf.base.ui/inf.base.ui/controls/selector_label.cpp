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
  g.setFont(getFont());
  g.setColour(findColour(inf_look_and_feel::colors::selector_label_text));
  g.saveState();
  if(_vertical) 
  {
    auto transform = AffineTransform().rotated(-pi32 / 2.0f, area.getWidth() / 2.0f, area.getHeight() / 2.0f);
    g.addTransform(transform);
    area = area.transformedBy(transform);
    area = juce::Rectangle<int>(area.getX() - vhpad, area.getY() + vhpad, area.getWidth() + vhpad, area.getHeight() - vhpad);
  }
  g.drawText(getText(), area, Justification::centred, false);
  g.drawArrow(Line<float>(0.0f, 10.0f, 10.0f, 10.0f), 1, 5, 5);
  g.restoreState();
}

} // namespace inf::base::ui