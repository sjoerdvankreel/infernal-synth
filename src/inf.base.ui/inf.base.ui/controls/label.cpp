#include <inf.base.ui/controls/label.hpp>

using namespace juce;

namespace inf::base::ui {

void 
inf_label::resized()
{
  Label::resized();
  check_bounds_warning();
}

void 
inf_label::textWasChanged()
{
  Label::textWasChanged();
  check_bounds_warning();
}

void
inf_label::check_bounds_warning()
{
#ifndef NDEBUG
  if (_bounds_warning_fired) return;
  if (!_perform_bounds_check) return;
  auto text_width = getFont().getStringWidth(getText());
  auto text_area = getBorderSize().subtractedFrom(getLocalBounds());
  if (text_area.getWidth() <= 0 || text_width < text_area.getWidth()) return;
  juce::AlertWindow::showMessageBoxAsync(MessageBoxIconType::WarningIcon, "Label exceeds bounds", getText());
  _bounds_warning_fired = true;
#endif
}

} // namespace inf::base::ui