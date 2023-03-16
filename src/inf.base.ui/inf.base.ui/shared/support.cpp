#include <inf.base.ui/shared/support.hpp>

using namespace VSTGUI;

namespace inf::base::ui {

inf::base::ui_color
from_vst_color_name(std::string const* name, IUIDescription const* desc)
{
  CColor color;
  assert(name != nullptr);
  bool ok = UIViewCreator::stringToColor(name, color, desc);
  (void)ok;
  assert(ok);
  return from_vst_color(color);
}

} // namespace inf::base::ui